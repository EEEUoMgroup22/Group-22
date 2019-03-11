#include "mbed.h"
#include "C12832.h"
#include "QEI.h"

class Potentiometer { 
private: 
 AnalogIn inputSignal; 
 float VDD, currentSamplePC, currentSampleVolts; 
public: 
 Potentiometer(PinName pin, float v) : inputSignal(pin), VDD(v) {} 
 
 float amplitudeVolts(void) { return (inputSignal.read()*VDD); } 
 float amplitudePC(void) { return inputSignal.read(); } 
 void sample(void) { 
 currentSamplePC = inputSignal.read(); 
 currentSampleVolts = currentSamplePC * VDD; 
 } 
 float get_volts(void) { return currentSampleVolts; } 
 float getCurrentSamplePC(void) { return currentSamplePC; } 
 
}; 

class motor_control{
    
    private:
    DigitalOut control_mode, direc;
    float pwm_period, pwm_duty;
    PwmOut motor;
    
    public:
    motor_control( int mode, int direction ,PinName pin_control_mode,PinName pin_direc, PinName pin_motor): control_mode(pin_control_mode), direc(pin_direc), motor(pin_motor)
    { 
    if (mode) { unipolar();} 
        else{ bipolar();}
    if (direction){ forward();}
    else{ backward();}
    };
    ~motor_control() { };
    
    void unipolar (){ 
            control_mode = 0;
        };
    void bipolar (){ 
       control_mode = 1;
    };
        
    void set_pwm(float period, float duty){
        pwm_period = period;
        pwm_duty = duty; 
        motor.period(pwm_period);
        motor.write(pwm_duty);
        };
        
    void forward (){ 
    direc = 0;
        };
    void backward() { 
    direc = 1;
        };
        
    };
    
    
    // create function to turn off and on enable
   class double_motor_control{
       private:
       float left_volts, right_volts, right_duty, left_duty, speed_gradient, pwm_period;
       Potentiometer left_speed_knob, right_speed_knob;
       motor_control motor_left, motor_right;
       public:
       double_motor_control(float period):left_speed_knob(A0, 3.3), right_speed_knob(A1, 3.3),
       motor_left( 1, 0,PA_11, PB_2, PB_14), motor_right( 1, 0,PB_12, PB_1, PB_13)
       { 
       pwm_period = period;
       speed_gradient = 1/3.3;
       right_duty=0;
       left_duty=0;
            };
       ~double_motor_control(){ };
       void sample_pots(){
          left_speed_knob.sample();
          right_speed_knob.sample();
           }  
           
       void update_speed(){
           sample_pots();
           left_duty = 1- speed_gradient*left_speed_knob.get_volts();
           right_duty = 1- speed_gradient*right_speed_knob.get_volts();
           motor_right.set_pwm(pwm_period, right_duty);
           motor_left.set_pwm(pwm_period, left_duty); 
           };
           float get_right_duty(){ return right_duty;};
           float get_left_duty(){ return left_duty;};
           
       void rotational_speed(){ };
       void translational_speed(){ };
       void square(){
           int a, i;
           for( a = 0; a<6; a++){
           motor_left.set_pwm(pwm_period, 0.8);
           motor_left.forward();
           motor_right.set_pwm(pwm_period, 0.8);
           motor_right.forward();
           wait(3);
           motor_left.set_pwm(pwm_period,1);
           wait(2);
           }
           // move forward at low speed for 3 seconds
           // turn left wheel off and turn right wheel on for 2 second 
           // repeat for 4 more times 
           motor_left.set_pwm(pwm_period, 0.6);
           motor_left.backward();
           motor_right.set_pwm(pwm_period, 0.6);
           motor_right.forward();
           wait(2);
           // rotate 180 by turning left wheel backward and right wheel forward for 2 second 
           
           //repeat 1 to 3 with turning left wheel on and right wheel off
            for(int i = 0; i <6; i++){
           motor_left.set_pwm(pwm_period, 0.8);
           motor_left.forward();
           motor_right.set_pwm(pwm_period, 0.8);
           motor_right.forward();
           wait(3);
           motor_right.set_pwm(pwm_period,1);
           wait(2);
           }
           //stop
           motor_left.set_pwm(pwm_period,1);
           motor_right.set_pwm(pwm_period,1);
       };
  };
       // enable pin
      // DigitalOut onewire(PC_8);
      C12832 lcd(D11, D13, D12, D7, D10);
       
class speed{
    
    private:
    Ticker timer; 
    int prev_pulses_count, current_pulses_count;
    QEI motor_speed; // where i use the QEI class
    float wheel_radius, sample_time, wheel_speed, tick_rate;
    
    public:
    speed(float stime, PinName chA, PinName chB): prev_pulses_count(0), current_pulses_count(0),motor_speed(chA, chB, NC, 256),wheel_radius(0.04),
    sample_time(stime){ 
          sample_time = stime;
          timer.attach(callback( this, &speed::calc_wheel_speed), sample_time); 
          motor_speed.reset(); // resets pulse count to 0
          };
    ~speed(){ };
       // get functions
       int get_wheel_pulses(){return motor_speed.getPulses();};
       float get_sample_time(){ return sample_time;};
       float get_wheel_speed(){ return wheel_speed;};
       
       void calc_wheel_speed(){
            current_pulses_count = get_wheel_pulses();
            tick_rate = (current_pulses_count - prev_pulses_count)/sample_time;
            wheel_speed = ((tick_rate)*wheel_radius)/(256*2); // 256 is the counts per revolution but we use it in X2 mode so we get 512counting at every rising edge
            prev_pulses_count = current_pulses_count;
            //timer.attach(callback(this, &speed::write_speed), sample_time);
            };
    };
    
         
int main() {
   DigitalOut enable(PB_15);
   enable = 1;
   double_motor_control motors(0.001); // try to put frequency 1000hz next time and see if beeping ocurrs
   speed motor_right(0.0005, PC_14, PC_15);
   speed motor_left(0.0005, PC_10, PC_12);
 // motor_control object( for unipolar = 1 for bipolar = 0, for forward = 1 for backward = 0, etc. )
 
 /*motor_control motor_left( 1, 0,PA_11, PB_2, PB_14);
 motor_left.set_pwm(0.01, 0.3);

 motor_control motor_right( 1, 0,PB_12, PB_1, PB_13);*/
 //motor_right.set_pwm(0.01, 0.00);
 // declare digital out for enable pin and turn on from here
   

 lcd.cls();
 while(true) {
     motors.update_speed();
  //clear the screen
 lcd.locate(0,0); //locate at (0,0)
 lcd.printf("left duty = %.2f",  1- motors.get_left_duty());
  lcd.locate(0,10); //locate at (0,10)+
 lcd.printf("right duty = %.2f", 1 - motors.get_right_duty());
 lcd.locate(0,20);
 lcd.printf("speed right = %.4f", motor_left.get_wheel_speed());
 }
 
}

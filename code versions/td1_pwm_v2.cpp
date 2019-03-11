#include "mbed.h"
#include "C12832.h"
//#include "QEI.h"

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
/*
class speed{
    
    private:
    Ticker timer; 
    int tick_rate, prev_pulses_count, current_pulses_count, wheel_speed, sample_time;
    QEI motor_speed;
    const int wheel_radius;
    C12832 lcd;
    
    public:
    speed(int stime): motor_speed(PA_5, PB_3, NC, 256), lcd(D11,D13,D12,D7,D10), wheel_radius(0.04){ 
          sample_time = stime;
          prev_pulses_count = 0;
          timer.attach(callback(this, &speed::write_speed), sample_time); 
          };
    ~speed(){ };
    
    int get_wheel_pulses(){
        return motor_speed.getPulses();
        };
        
       void write_speed(){
            current_pulses_count = get_wheel_pulses();
            wheel_speed = ((( current_pulses_count - prev_pulses_count)/sample_time)*wheel_radius)/(256*2); // 256 is the counts per revolution but we use it in X2 mode so we get 512counting at every rising edge
            lcd.cls();
            lcd.locate(8,20);
            lcd.printf("%f", wheel_speed);
            prev_pulses_count = current_pulses_count;
            };
    };
*/
class motor_control{
    
    private:
    DigitalOut control_mode, direc;
    float pwm_period, pwm_duty;
    PwmOut motor;
    
    void forward (){ 
    direc = 0;
        };
    void backward() { 
    direc = 1;
        };
        
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
       };
       
       DigitalOut enable(PB_15); // enable pin
       DigitalOut onewire(PC_8);
      C12832 lcd(D11, D13, D12, D7, D10);
       
int main() {
 enable = 1;
 onewire = 0;
 double_motor_control motors(0.01); // try to put frequency 1000hz next time and see if beeping ocurrs
 
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
  lcd.locate(0,20); //locate at (0,0)
 lcd.printf("right duty = %.2f", 1- motors.get_right_duty());
 }
 
}

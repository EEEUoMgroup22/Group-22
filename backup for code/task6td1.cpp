#include "mbed.h"
#include "C12832.h"
#include "QEI.h" 

 C12832 lcd(D11, D13, D12, D7, D10);

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
        
    }; // end of class
    
    
class speed{
    
    private:
    //Ticker timer; 
    int prev_pulses_count, current_pulses_count;
    QEI motor_speed; // where i use the QEI class
    float wheel_radius, sample_time, wheel_speed, tick_rate;
    
    public:
    speed(float stime, PinName chA, PinName chB): prev_pulses_count(0), current_pulses_count(0),motor_speed(chA, chB, NC, 256),wheel_radius(0.04),sample_time(stime){
        //  timer.attach(callback(this, &speed::calc_wheel_speed), sample_time);  
          motor_speed.reset(); // resets pulse count to 0
          };
    ~speed(){ };
    
       int get_wheel_pulses(){return motor_speed.getPulses();};
       
       float get_sample_time(){ return sample_time;};
       void reset(){ motor_speed.reset();}
       float calc_wheel_speed(){
            current_pulses_count = get_wheel_pulses();
            tick_rate = (current_pulses_count - prev_pulses_count)/sample_time;
            wheel_speed = ((tick_rate)*wheel_radius)/(256*2); // 256 is the counts per revolution but we use it in X2 mode so we get 512counting at every rising edge
            prev_pulses_count = current_pulses_count;
            return wheel_speed;
            //timer.attach(callback(this, &speed::write_speed), sample_time);
            };
    };// end of class
    // create function to turn off and on enable
    
    
class double_motor_control{
       
       private:
       motor_control motor_left, motor_right;
       QEI encmotor_right;
       QEI encmotor_left;
       float left_volts, right_volts, right_duty, left_duty, speed_gradient, pwm_period;
       int a, i, count, right_pulses, left_pulses;
       //Potentiometer left_speed_knob, right_speed_knob;
       Ticker timer;
       
       public:
       double_motor_control(float period, float sample_time):
       motor_left( 1, 1,PA_11, PB_2, PB_14), motor_right( 1, 1,PB_12, PB_1, PB_13),encmotor_right( PC_14, PC_15,NC, 256),encmotor_left(PC_10, PC_12, NC, 256),
       right_duty(0),left_duty(0),right_pulses(0),left_pulses(0), a(0),i(0), count(0)
       { 
       pwm_period = period;
       speed_gradient = 1/3.3;
       right_duty=0;
       left_duty=0;
       right_pulses = 0;
       left_pulses = 0;
       timer.attach(callback(this, &double_motor_control::update_wheel_pulses), sample_time); 
       motor_left.set_pwm(pwm_period, 1);
       motor_right.set_pwm(pwm_period, 1);
            };
       ~double_motor_control(){ };
           
   // everything pulses    
   void update_wheel_pulses(){
           right_pulses = -(encmotor_right.getPulses());    
           left_pulses = encmotor_left.getPulses();
           };
   void zero_pulses(){ 
   ++count;
   encmotor_right.reset();
   encmotor_left.reset();
   right_pulses = 0;
   left_pulses = 0;
   lcd.locate(0,20);
   lcd.printf("%d",count);
   };
   float get_right_duty(){ return right_duty;};
   float get_left_duty(){ return left_duty;};
   int get_left_wheel_pulse(){return left_pulses;};
   int get_right_wheel_pulse(){return right_pulses;};

   void rotational_speed(){ };
   void translational_speed(){ };
   void rotation (){
           motor_left.set_pwm(pwm_period, 0.5);
           motor_left.forward();
           motor_right.set_pwm(pwm_period, 0.5);
           motor_right.forward();
           wait(2);
           motor_left.set_pwm(pwm_period, 1);
           motor_right.set_pwm(pwm_period, 1);
           }
           
    void left(float duty){ motor_left.set_pwm(pwm_period, duty);};
    void right(float duty){ motor_right.set_pwm(pwm_period, duty);};
    
    void go_forward(int Pulses){
          while (right_pulses< Pulses  ){
           motor_left.set_pwm(pwm_period, 0.75);
           motor_left.forward();
           motor_right.set_pwm(pwm_period, 0.745);
           motor_right.forward(); 
           lcd.locate(0,0);
           lcd.printf("a = %d, i = %d", a, i);
           }
           motor_left.set_pwm(pwm_period, 1);
           motor_right.set_pwm(pwm_period, 1);
          };
           
    void rotate180(){
           zero_pulses();
           motor_right.set_pwm(pwm_period,1);
      while( left_pulses < 570){
           lcd.locate(0,0);
           lcd.printf("a = %d, i = %d", a, i);
           motor_left.set_pwm(pwm_period, 0.78);
           motor_left.forward();
           }
           motor_left.set_pwm(pwm_period, 1);
           motor_right.set_pwm(pwm_period, 1);
           zero_pulses();
           wait(1);
     while( right_pulses > -550){
           lcd.locate(0,0);
           lcd.printf("a = %d, i = %d", a, i);
           motor_right.set_pwm(pwm_period, 0.775);
           motor_right.backward();
           }
           motor_left.set_pwm(pwm_period, 1);
           motor_right.set_pwm(pwm_period, 1);
           zero_pulses();
           
           };

    void anti_rotate90(){
           zero_pulses();
      while( right_pulses < 560){
           lcd.locate(0,0);
           lcd.printf("a = %d, i = %d", a, i);
           motor_left.set_pwm(pwm_period,1);
           motor_right.set_pwm(pwm_period, 0.8);
           motor_right.forward();
           
           }
           motor_left.set_pwm(pwm_period, 1);
           motor_right.set_pwm(pwm_period, 1);
           };   
              
   void rotate90(){
           zero_pulses();
      while( left_pulses < 560){
           lcd.locate(0,0);
           lcd.printf("a = %d, i = %d", a, i);
           motor_right.set_pwm(pwm_period,1);
           motor_left.set_pwm(pwm_period, 0.8);
           motor_left.forward();
           }
           motor_left.set_pwm(pwm_period, 1);
           motor_right.set_pwm(pwm_period, 1);
           }; 
             
     void square(){
       //lcd.locate(0,10);
      // lcd.printf("pulse left right = %d    %d", left_pulses,right_pulses);
       //lcd.locate(0,0);
       //lcd.printf("a = %d, i = %d", a, i);
           // repeat for 4 more times 
       int pulse = 750;
       encmotor_left.reset();
       encmotor_right.reset();
    
           // forward 0.5m
          go_forward( pulse);
          zero_pulses();
          wait(0.5);
           // rotate 90 clockwise
          rotate90();
          zero_pulses();
           
          go_forward(570);
          zero_pulses();
          wait(0.5);
          rotate90();
          zero_pulses();
          
           go_forward(570);
          zero_pulses();
          wait(0.5);
          
          
          rotate90();
          go_forward(570);
          
          wait(0.5);
          rotate180();
    };
             
  void square2(){
    int pulse = 750;
       encmotor_left.reset();
       encmotor_right.reset();
    
           // forward 0.5m
          go_forward(900);
          zero_pulses();
          wait(0.5);
           // rotate 90 anticlockwise
          anti_rotate90();
          zero_pulses();
          // forward 0.5m 
          go_forward(570);
          zero_pulses();
          wait(0.5);
        // rotate 90 anticlockwise
          anti_rotate90();
          zero_pulses();
          // forward 0.5m
           go_forward(570);
          zero_pulses();
          wait(0.5);
         // rotate 90 anticlockwise
          anti_rotate90();
          zero_pulses();
          go_forward(750);
          zero_pulses();
    };
    
  };// end of class
  
       DigitalOut enable(PB_15); // enable pin
         
int main() {
 enable = 1;
 //onewire = 0;
 double_motor_control motors(0.001, 0.0001); // try to put frequency 1000hz next time and see if beeping ocurrs
/*  int left_pulse, right_pulse, a, i, test;
 QEI encleft(PC_10, PC_12, NC, 256), encright(PC_14, PC_15, NC, 256);
 motor_control motor_left( 1, 0,PA_11, PB_2, PB_14);
 motor_control motor_right( 1, 0,PB_12, PB_1, PB_13);*/

 // motor_control object( for unipolar = 1 for bipolar = 0, for forward = 1 for backward = 0, etc. )
 
 /*motor_control motor_left( 1, 0,PA_11, PB_2, PB_14);
 motor_left.set_pwm(0.01, 0.3);

 motor_control motor_right( 1, 0,PB_12, PB_1, PB_13);*/
 //motor_right.set_pwm(0.01, 0.00);
wait(1);
motors.square();
//motors.rotate180();
wait(0.5);
motors.square2();

 while(true) {

     lcd.locate(80,0);
     lcd.printf("hi");
    /* motors.update_speed();
  //clear the screen
 lcd.locate(0,0); //locate at (0,0)
 lcd.printf("left duty = %.2f",  1- motors.get_left_duty());
  lcd.locate(0,20); //locate at (0,0)
 lcd.printf("right duty = %.2f", 1- motors.get_right_duty());*/
 }
 
}

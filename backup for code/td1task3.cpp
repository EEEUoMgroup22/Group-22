#include "mbed.h"
#include "C12832.h"
#include "QEI.h"


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
       
       float calc_wheel_speed(){
            current_pulses_count = get_wheel_pulses();
            tick_rate = (current_pulses_count - prev_pulses_count)/sample_time;
            wheel_speed = ((tick_rate)*wheel_radius)/(256*2); // 256 is the counts per revolution but we use it in X2 mode so we get 512counting at every rising edge
            prev_pulses_count = current_pulses_count;
            return wheel_speed;
            //timer.attach(callback(this, &speed::write_speed), sample_time);
            };
    };
    
    
    int wheel_left_pulses_A,wheel_left_pulses_B,wheel_right_pulses_A,wheel_right_pulses_B;
    void inc_left_wheel_pulses_A(){ ++wheel_left_pulses_A;};
    void inc_left_wheel_pulses_B(){ ++wheel_left_pulses_B;};
    int get_wheel_left_pulses_A(){ return wheel_left_pulses_A;};
    int get_wheel_left_pulses_B(){ return wheel_left_pulses_B;};
    InterruptIn pulses_left_A(PH_0), pulses_left_B(PH_1), pulses_right_A(PB_5), pulses_right_B(PB_3);
    
int main() {
    // pulse per revolution = 256
     speed motor_right(0.01, PC_14, PC_15);
     speed motor_left(0.01, PC_10, PC_12);
    // the pins i used for qei class external from class
    //QEI motor_speed_left(PB_3, PA_10, NC, 256);
    C12832 lcd(D11,D13,D12,D7,D10);
    // where i used interruptin
    lcd.cls();

    while(1) {
       lcd.locate(0,0);
       lcd.printf("left pulses = %.4d", motor_left.get_wheel_pulses());
       lcd.locate(0,10);
       lcd.printf("right pulses = %.4d", -(motor_right.get_wheel_pulses()));
    /* pulses_left_A.rise(&inc_left_wheel_pulses_A);
      pulses_left_B.rise(&inc_left_wheel_pulses_B);
     lcd.locate(0,0);
     
     lcd.printf("%d", get_wheel_left_pulses_A());
     lcd.locate(0,10);
     lcd.printf("%d", get_wheel_left_pulses_B());*/
    }
}

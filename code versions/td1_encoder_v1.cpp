#include "mbed.h"
#include "C12832.h"
#include "QEI.h"


class speed{
    
    private:
    Ticker timer; 
    int tick_rate, prev_pulses_count, current_pulses_count, wheel_speed, sample_time;
    QEI motor_speed;
     float wheel_radius;
    C12832 lcd;
    
    public:
    speed(float stime): motor_speed(PB_3, PA_3, NC, 256), lcd(D11,D13,D12,D7,D10){ 
          sample_time = stime;
          prev_pulses_count = 0;
          timer.attach(callback(this, &speed::write_speed), sample_time);
          lcd.cls(); 
          wheel_radius = 0.04;
          };
    ~speed(){ };
    
    int get_wheel_pulses(){
        return motor_speed.getPulses();
        };
        
       void write_speed(){
            current_pulses_count = get_wheel_pulses();
            wheel_speed = ((( current_pulses_count - prev_pulses_count)/sample_time)*wheel_radius)/(256*2); // 256 is the counts per revolution but we use it in X2 mode so we get 512counting at every rising edge
            lcd.locate(0,0);
            lcd.printf("%f", 0);
            prev_pulses_count = current_pulses_count;
            };
    };
    

int main() {
    // pulse per revolution = 256
    speed motor_left(0.05);
    while(1) {
     
    }
}

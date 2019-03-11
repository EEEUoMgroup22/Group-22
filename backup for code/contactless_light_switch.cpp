#include "mbed.h"
#include "C12832.h"

int fall = 0;

void toggle(DigitalOut &LED, int & light_state){
   if ( fall == 1){ 
    light_state != light_state;
    LED = light_state;
    }
 }
 
  void falling_edge(){
      fall = 1;
      }
  void rise_edge(){
      fall = 0;
      }
      
  int compare_voltage(DigitalIn &led){
      if(led > sample_voltage){
         return 1;}
         else{return 0;}
         }
         
int main() {
    InterruptIn TCRT(PA_3);
    TCRT.rise(&rise_edge);
    TCRT.fall(&falling_edge);
    int light_state = 0;
    C12832 lcd(D11,D13,D12,D7,D10);
    DigitalOut light(PA_2);
    DigitalIn TCRT5000(PC_4);
    
    while(1) {
        toggle(light, light_state);
        lcd.locate(0,0);
        lcd.printf("light state = %d", light_state);
    }
}

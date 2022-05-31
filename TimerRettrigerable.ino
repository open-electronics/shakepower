
#include <avr/sleep.h>
#include <PinChangeInterrupt.h>
#include <EEPROM.h>



// DEFINIZIONE PIN
#define TILT_SENSOR 1
#define BUZZER 2
#define POWER_EN 4
#define LED1 3



#define PRESSED LOW
#define NOT_PRESSED HIGH


const unsigned long shortPress = 100;
const unsigned long longPress = 2000;
long blinkInterval = 500;
unsigned long previousBlink=0;


unsigned int m_status;
unsigned int n_interval = 2;

bool power_status   = false;
bool led_tilt_status = false;

unsigned long timer = 0;
unsigned long timer_led_tilt = 0;
unsigned long timer_buzzer = 0;
unsigned long countDown = 1000L;



typedef struct Buttons {
    const byte pin = 0;
    const int debounce = 10;
    unsigned long counter=0;
    bool prevState = NOT_PRESSED;
    bool currentState;
} Button;


// create a Button variable type
Button button;







void sleep() {
    digitalWrite(POWER_EN , HIGH);  
    digitalWrite(LED1, LOW); 
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    
    sleep_enable();                        
    sei();                                  
    sleep_cpu();                            
} 



void irs_sub(void) {
  power_status  =  true;
  led_tilt_status =true;
  timer = millis(); 
  timer_led_tilt = millis();
  timer_buzzer = millis(); 
}



void beepLong(){
  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);
}


void beepShort(){
  digitalWrite(BUZZER, HIGH);
  delay(100);
  digitalWrite(BUZZER, LOW);
}



void blinkLong(){
  digitalWrite(LED1, HIGH);
  delay(500);
  digitalWrite(LED1, LOW);
}


void blinkShort(){
  digitalWrite(LED1, HIGH);
  delay(100);
  digitalWrite(LED1, LOW);
}


void notify_delay(){
 if (n_interval != 255) {
    for(int x = 0; x < n_interval; x++) {
      beepShort();
      blinkShort();
      delay(1000);  
    }
  }  
}



void handleLongPress() {
  
   if (m_status == 0) { 
     beepLong();
     blinkLong();   
     n_interval  = 1;  
     m_status = 1;
   }
   
   if (m_status == 1 && n_interval > 1) {
     notify_delay();
     EEPROM.write(0, n_interval);      
     m_status = 0;     
   }  
    
}



void handleShortPress() {
   if (m_status == 1) {
     n_interval  = n_interval + 1;  
     beepShort(); 
     blinkShort();
   }     
}



void Handle_Config(){
  
   button.currentState = digitalRead(button.pin);
    
    if (button.currentState != button.prevState) {     
        delay(button.debounce); 
            
        button.currentState = digitalRead(button.pin);  
        
        if (button.currentState == PRESSED) {           
            button.counter = millis();
        }
        
        if (button.currentState == NOT_PRESSED) {    
            unsigned long currentMillis = millis();              
            if ( (currentMillis - button.counter >= shortPress) && !(currentMillis - button.counter >= longPress) )  {     
                handleShortPress();                          
            }
            if ((currentMillis - button.counter >= longPress)) {    
                handleLongPress();
            }                 
        }  
                
        button.prevState = button.currentState;  
            
    } 
     
}







void Handle_Status(){
  
  if (m_status == 0) { 
         
        if (power_status){ 
         digitalWrite(POWER_EN , LOW);  
        }
    
        if (led_tilt_status){ 
          digitalWrite(LED1, HIGH);      
        }else {
          digitalWrite(LED1, LOW); 
        }
    
        if (millis()- timer_led_tilt >=  10 ){     
         led_tilt_status=false;
         timer_led_tilt = millis();
        }
    
        if ((millis() - timer) >= countDown) {  
          power_status=false; 
          //timer = millis();            
          sleep();    
        }
        
    }else {
      digitalWrite(POWER_EN , HIGH);
      digitalWrite(LED1, LOW); 
    }
    
}





void setup() {

  pinMode(POWER_EN , OUTPUT);
  digitalWrite(POWER_EN , HIGH);

  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);
  
  
  pinMode(button.pin, INPUT_PULLUP);

  pinMode(TILT_SENSOR, INPUT);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  m_status = 0;
  
  n_interval = 1;

  if (EEPROM.read(0) != 255){
    n_interval = EEPROM.read(0);
  }
  
  countDown = (unsigned long) n_interval * 3600 * 1000L;

  notify_delay();

  attachPCINT(digitalPinToPCINT(TILT_SENSOR), irs_sub , FALLING);
  
  timer = millis(); 
  timer_led_tilt = millis();
  timer_buzzer = millis();
    
}






void loop() { 

    Handle_Config();

    Handle_Status();
   
}

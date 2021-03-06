// Enable debug prints to serial monitor

#define MY_DEBUG 

// Enable and select radio type attached
//#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

// Set LOW transmit power level as default, if you have an amplified NRF-module and
// power your radio separately with a good regulator you can turn up PA level. 
//#define MY_RF24_PA_LEVEL RF24_PA_LOW

// Enable serial gateway
#define MY_GATEWAY_SERIAL
// Define a lower baud rate for Arduino's running on 8 MHz (Arduino Pro Mini 3.3V & SenseBender)
#if F_CPU == 8000000L
#define MY_BAUD_RATE 38400
#endif

// Flash leds on rx/tx/err
// #define MY_LEDS_BLINKING_FEATURE
// Set blinking period
// #define MY_DEFAULT_LED_BLINK_PERIOD 300

// Inverses the behavior of leds
// #define MY_WITH_LEDS_BLINKING_INVERSE

// Enable inclusion mode
#define MY_INCLUSION_MODE_FEATURE
// Enable Inclusion mode button on gateway
#define MY_INCLUSION_BUTTON_FEATURE

// Inverses behavior of inclusion button (if using external pullup)
//#define MY_INCLUSION_BUTTON_EXTERNAL_PULLUP

// Set inclusion mode duration (in seconds)
#define MY_INCLUSION_MODE_DURATION 60 
// Digital pin used for inclusion mode button
#define MY_INCLUSION_MODE_BUTTON_PIN  3 

// Uncomment to override default HW configurations
//#define MY_DEFAULT_ERR_LED_PIN 4  // Error led pin
//#define MY_DEFAULT_RX_LED_PIN  6  // Receive led pin
//#define MY_DEFAULT_TX_LED_PIN  5  // the PCB, on board LED
#define MY_NODE_ID 1
#include <SPI.h>
#include <MySensors.h>  
#include <Bounce2.h>

// Enable repeater functionality for this node
#define MY_REPEATER_FEATURE
#define PUSHINTERVAL 5

#define RELAY_ON 1
#define RELAY_OFF 0

class Light {
  public:
    Light(int, int, int);
    void Present();
    void LightUpdate();
    void RecieveMessage(MyMessage);
    void LoadState();
    
  private:
    int Relay, Button, ID;
    Bounce debouncer = Bounce();
    MyMessage msg;
};
//Light::Light(int ID, int Relay, int Button){
Light::Light(int ID, int Button, int Relay){
  this->Relay = Relay;
  this->Button = Button;
  this->ID = ID;
  this->msg = MyMessage(this->ID, V_LIGHT);

  //initialize button
  pinMode(this->Button, INPUT_PULLUP);
  this->debouncer = Bounce();
  this->debouncer.attach(this->Button);
  this->debouncer.interval(PUSHINTERVAL);

  //initialize relay
  pinMode(this->Relay, OUTPUT);
  //digitalWrite(this->Relay, RELAY_OFF); //Turn off Light by default on startup
}
void Light::LoadState(){
  digitalWrite(this->Relay, loadState(this->ID)?RELAY_ON:RELAY_OFF);
  send(this->msg.set(loadState(this->ID)));
}
void Light::Present(){
  present(this->ID, S_BINARY);
}
void Light::LightUpdate(){
  
  if (this->debouncer.update()) {  
    int value = debouncer.read();
    if(value == LOW){
         saveState(this->ID, !loadState(this->ID));
         digitalWrite(this->Relay, loadState(this->ID)?RELAY_ON:RELAY_OFF);
         send(this->msg.set(loadState(this->ID)));
    }
  }
}
void Light::RecieveMessage(MyMessage message){
  digitalWrite(this->Relay, message.getBool()?RELAY_ON:RELAY_OFF);
  saveState(message.sensor, message.getBool());
}

//
Light Lights[] = {
   Light(0,2,53),
   Light(1,3,52),
   Light(2,4,51),
   Light(3,5,50), 
   Light(4,6,49), 
   Light(5,7,48),
   Light(6,8,47), 
   Light(7,9,46), 
   Light(8,10,45),
   Light(9,11,44), 
   Light(10,12,43), 
   Light(11,13,42),
   Light(12,14,41),
   Light(13,15,40),
   Light(14,16,39),
   Light(15,17,38),
   Light(16,18,37),
   Light(17,19,36),
   Light(18,20,35),
   Light(19,21,34),
   Light(20,22,33),
   Light(21,23,32),
   Light(22,24,31),
   Light(23,25,30),
   Light(24,26,29),
   Light(25,27,28)
};
unsigned int NumberOfLights = 26;

void before() { 

}
void setup() { 
    for (int x = 0; x<NumberOfLights; x++) {
      Lights[x].LoadState();
    }
}
void presentation()  
{   
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Light driver!", "1.0");
    for (int x = 0; x<NumberOfLights; x++) {
      Lights[x].Present();
    }
}

void loop() {
  
    for (int x = 0; x<NumberOfLights; x++) {
      Lights[x].LightUpdate();
    }
}

void receive(const MyMessage &message) {
  if (message.type==V_LIGHT) {
     int sensorID = message.sensor;
     Lights[sensorID].RecieveMessage(message);
   } 
}

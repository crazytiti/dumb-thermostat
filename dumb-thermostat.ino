// Include the libraries we need
#include <OneWire.h>
#include "DallasTemperature.h"
#include <LiquidCrystal.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 6
#define Chauffage_Plus 10
#define Chauffage_Moins 9
#define Boutton_Plus 7
#define Boutton_Moins 8

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// consigne de température
float consigne = 21.1;
float temp_actuelle;
float compensation = -0.5;
float hysteresis = 0.2;

int i = 0;
int chauffe_actif = 0;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

/*
 * The setup function. We only start the sensors here
 */
void setup(void)
{
  // set up the LCD's number of columns and rows:
  lcd.begin(8, 1);
  // Print a message to the LCD.
  lcd.print("Init");
  pinMode(Chauffage_Plus, OUTPUT);
  pinMode(Chauffage_Moins, OUTPUT);
  pinMode(Boutton_Plus, INPUT);
  pinMode(Boutton_Moins, INPUT);
  // start serial port
  Serial.begin(115200);
  Serial.println("Dumb Thermostat");
  // Start up the library
  sensors.begin();
  //test du relais
  digitalWrite(Chauffage_Plus, HIGH);
  digitalWrite(Chauffage_Moins, LOW);  
  delay(100);
  digitalWrite(Chauffage_Plus, LOW);
  digitalWrite(Chauffage_Moins, HIGH);  
  delay(100);    
  digitalWrite(Chauffage_Moins, LOW); 
}

/*
 * Main function, 
 */
void loop(void)
{ 
  if(i>100){
    Control_Temp();
    i=0;
  }
  if(digitalRead(Boutton_Plus)){
    consigne +=0.1;
    lcd.clear();
    lcd.print(consigne,1);
    delay(300);
  }
  if(digitalRead(Boutton_Moins)){
    consigne -=0.1;
    lcd.clear();
    lcd.print(consigne,1);
    delay(300);
  }
  delay(10);
  i++;
}

void Control_Temp(void){
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  Serial.print("Temperature for the device 1 is: ");
  temp_actuelle = sensors.getTempCByIndex(0);
  temp_actuelle += compensation;
  Serial.println(temp_actuelle);  
  Serial.print("Consigne is: ");
  Serial.println(consigne);
  lcd.clear();
  lcd.print(temp_actuelle,1);
  lcd.print(' ');
  if(chauffe_actif){
    if(consigne < temp_actuelle - hysteresis){
      digitalWrite(Chauffage_Plus, LOW);
      digitalWrite(Chauffage_Moins, HIGH);  
      delay(100);    
      digitalWrite(Chauffage_Moins, LOW);  
      Serial.print("--Heater OFF--");
      chauffe_actif = 0;
      lcd.print('_');
    }
    else{
      lcd.print('*');
    }
  }
  else{
    if(consigne > temp_actuelle + hysteresis){
      digitalWrite(Chauffage_Plus, HIGH);
      digitalWrite(Chauffage_Moins, LOW);  
      delay(100);
      digitalWrite(Chauffage_Plus, LOW);
      Serial.print("--Heater ON--");
      chauffe_actif = 1;
      lcd.print('*');
    }
    else{
      lcd.print('_');
    }
  }
  
}

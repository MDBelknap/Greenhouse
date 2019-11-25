#include "DHT.h"

#define DHTPIN 2  //Digital pin connected to DHT sensor

#define DHTTYPE DHT11 //DHT11 Sensor

int heaterPin = 3;    //connects to relay controlling the heater
int hydroSen1 = A1;   //connects to soil moisture sensor for zone 1
int hydroZone1 = 5;   //connects to solenoid valve for zone 1
int hydroSen2 = A2;   //connects to soil moisture sensor for zone 2
int hydroZone2 = 6;   //connects to solenoid valve for zone 2
int hydroSen3 = A3;   //connects to soil moisture sensor for zone 3
int hydroZone3 = 7;   //connects to solenoid valve for zone 3
int sensePower = 9;   //power for moisture sensors
int pump = 10;        //connects to relay controlling water pump
int photoRes = A0;    //connects to photo resistor
int growLight = 11;   //connects to relay controlling grow light
int counter = 0;      //variable for grow light timing

DHT dht(DHTPIN, DHTTYPE);


void setup() {
  Serial.begin(115200);
  Serial.println(F("Welcome to the Belknap's greenhouse control program"));

  pinMode(heaterPin, OUTPUT);
  pinMode(hydroSen1, INPUT);
  pinMode(hydroZone1, OUTPUT);
  pinMode(hydroSen2, INPUT);
  pinMode(hydroZone2, OUTPUT);
  pinMode(hydroSen3, INPUT);
  pinMode(hydroZone3, OUTPUT);
  pinMode(sensePower, OUTPUT);
  pinMode(pump, OUTPUT);
  pinMode(photoRes, INPUT);
  pinMode(growLight, OUTPUT);

  digitalWrite(growLight, LOW);
  digitalWrite(pump, LOW);
  digitalWrite(sensePower, LOW);
  counter == 0;

  dht.begin();
}

void loop() {


  delay(1000);

  float humid = dht.readHumidity();
  //read the humidity
  float tempF = dht.readTemperature(true);
  //reads Fahrenheit if true
  digitalWrite(sensePower, HIGH);
  //turns the power on to the moisture sensors
  int hydroLevel1 = analogRead(hydroSen1);
  //reads the soil moisture level of zone 1 as a number beteen 1023 and 300
  int hydroLevel2 = analogRead(hydroSen2);
  //reads the soil moisture level of zone 2 as a number beteen 1023 and 300
  int hydroLevel3 = analogRead(hydroSen3);
  //reads the soil moisture level of zone 3 as a number beteen 1023 and 300
  digitalWrite(sensePower, LOW);
  //turns the power off to the moisture sensors
  int light = analogRead(photoRes);
  //reading the light sensor resistor divider


  if (isnan(humid) || isnan(tempF)) {
    Serial.println(F("Failed to read temp/humidity sensor"));
    return;
    //sends error if DTH sensor is not functioning
  }

  float heatIndx = dht.computeHeatIndex(tempF, humid);
  Serial.print("Light: ");
  Serial.println(light);
  Serial.print(F("Humidity: "));
  Serial.print(humid);
  Serial.println("%");
  Serial.print(F("Temperature: "));
  Serial.print(tempF);
  Serial.println(F("*F"));
  Serial.print("Heat Index");
  Serial.print(heatIndx);
  Serial.println("*F");
  Serial.print("Soil Moisture Zone 1: ");
  Serial.print(hydroLevel1);
  Serial.print(", ");
  Serial.print("Soil Moisture Zone 2: ");
  Serial.print(hydroLevel2);
  Serial.print(", ");
  Serial.print("Soil Moisture Zone 3: ");
  Serial.print(hydroLevel3);
  Serial.println(", ");

  if (tempF <= 30) {

    Serial.println("Heater On");
    digitalWrite(heaterPin, HIGH);
  }
  else {
    Serial.println("Heater Off");
    digitalWrite(heaterPin, LOW);
  }
  if (hydroLevel1 >= 750) { //sets the moisture threshold for zone 1

    digitalWrite(hydroZone1, HIGH);
    delay(1000);
    digitalWrite(pump, HIGH);
    Serial.print("Zone 1: ON, ");
    delay(1000);  //sets how long the water pump runs, pump cap = 2 liters/min
    digitalWrite(pump, LOW);
    delay(1000);
    digitalWrite(hydroZone1, LOW);
    Serial.println("Zone 1: OFF");
    delay(10000);
  }
  else {
    digitalWrite(hydroZone1, LOW);
    digitalWrite(pump, LOW);
    Serial.println("Zone 1: WET, ");
  }
  if (hydroLevel2 >= 750) {

    digitalWrite(hydroZone2, HIGH);
    delay(1000);
    digitalWrite(pump, HIGH);
    Serial.print("Zone 2: ON, ");
    delay(3000);   //sets how long the water pump runs, pump cap = 2 liters/min
    digitalWrite(pump, LOW);
    delay(1000);
    digitalWrite(hydroZone2, LOW);
    Serial.println("Zone 2: OFF");
    delay(10000);
  }
  else {
    digitalWrite(hydroZone2, LOW);
    digitalWrite(pump, LOW);
    Serial.println("Zone 2: WET, ");

  }
  if (hydroLevel3 >= 750) {

    digitalWrite(hydroZone3, HIGH);
    delay(1000);
    //digitalWrite(pump, HIGH);
    Serial.print("Zone 3: ON, ");
    delay(100);   //sets how long the water pump runs, pump cap = 2 liters/min
    digitalWrite(pump, LOW);
    delay(1000);
    digitalWrite(hydroZone3, LOW);
    Serial.println("Zone 3: OFF");
    delay(10000);
  }
  else {
    digitalWrite(hydroZone3, LOW);
    digitalWrite(pump, LOW);
    Serial.println("Zone 3: WET");
  }

  if (light >= 300) {
    // sets the threshold for turning the grow light on
    Serial.print("The count is: ");
    Serial.println(counter);
    counter ++;  //adds one to the count every cycle of the program

    if (counter <= 5) {  //sets the length of time the grow light is on
      digitalWrite(growLight, HIGH);
      Serial.println("Growlight On");
    }
    else {
      digitalWrite(growLight, LOW);
      Serial.println("Growlight OFF");
    }
    if (counter == 10) {
      //set the length of time the light is kept off between cycles during nighttime
      counter = 0; //resets counter to enable another cycle of grow light
      Serial.println("the counter is reset to zero");
      //Serial.println(counter);
    }
  }
  else {
    digitalWrite(growLight, LOW);  //keeps the grow light off when there is daylightww
    Serial.println("Growlight OFF");
    counter = 0; //resets the counter to get a full cycle of growlight when light is triggered

  }


  delay(1000);  //
  Serial.println();
}

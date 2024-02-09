#include <DHT.h>
//version 1 calibration. s as soil calibration, w as water calibration 
const int s_dry_condition = 470; // value for dry sensor from calibration
const int s_wet_condition = 180; // value for wet sensor from calibration

const int w_dry_condition = 470; // value for dry sensor from calibration
const int w_wet_condition = 180; // value for wet sensor from calibration

int Pin_D1 = 5;
int Pin_D2 = 4;
int Pin_D5 = 14;
int readSoil;
int readWater;
int SoilSensor;
int WaterLevel;

#define DHTTYPE DHT11
DHT dht(Pin_D5, DHTTYPE);

void setup() {
  Serial.begin(9600); // Initialize serial communication at a baud rate of 9600
  pinMode(Pin_D1,OUTPUT);
  pinMode(Pin_D2,OUTPUT);
  pinMode(A0,INPUT);
  dht.begin();        // Initialize the DHT sensor
}

void loop() {

  // Read temperature and humidity data from the sensor
  float temperature = dht.readTemperature(); // Read temperature in Celsius
  float humidity = dht.readHumidity();       // Read humidity as a percentage

  // Check if the sensor reading is valid (sometimes returns NaN if reading is unsuccessful)
  if (!isnan(temperature) && !isnan(humidity)) {
    // Print the data to the Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C ");
    Serial.print(" Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  } else {
    Serial.println("Failed to read data from the sensor!");
  }

  delay(2000); // Delay for 2 seconds before taking the next reading

  //first sensor @ soil sensor
  digitalWrite(Pin_D1,HIGH); //turn on sensor 1
  delay(100);
  readSoil = analogRead(A0);
  digitalWrite(Pin_D1,LOW); //turn off sensor 1
  delay(100);
  
  //second sensor @ water sensor
  digitalWrite(Pin_D2,HIGH); //turn on sensor 2
  delay(100);
  readWater = analogRead(A0);
  digitalWrite(Pin_D2,LOW); //turn off sensor 2
  delay(100);
  
  SoilSensor=map(readSoil, s_wet_condition, s_dry_condition, 100, 0);
  WaterLevel=map(readWater, w_wet_condition, w_dry_condition, 100, 0);
  
  Serial.print("Soil = ");   
  Serial.println(readSoil);
  Serial.print(SoilSensor);
  Serial.println("%");
  delay(1000);
  
  Serial.print("Water = ");   
  Serial.println(readWater);
  Serial.print(WaterLevel);
  Serial.println("%");
  delay(1000);
  
}
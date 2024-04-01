//=======================================================PASTE FROM BLYNK APP========================================================================
#define BLYNK_TEMPLATE_ID "TMPL6Oh3bbEKX"
#define BLYNK_TEMPLATE_NAME "Plant Watering System"
#define BLYNK_AUTH_TOKEN "tbKTdpj-jhE6-aanwyBCj7U-Q2LR2YNc"
//===================================================END OF PASTE FROM BLYNK APP=====================================================================
#define BLYNK_PRINT Serial
//==============================================ALL LIBRARIES FOR WIFI & BLYNK CONNECTION============================================================
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp8266.h>
//==============================================================END PART=============================================================================
//========================================================ALL SENSOR PIN PART========================================================================
#include "DHT.h"
#define DHTTYPE DHT11             //--> Defines the type of DHT sensor used (DHT11, DHT21, or DHT22).
#define DHTPIN D3                 // TEMPERATURE & HUMIDITY PART
#define soil D1                   // SOIL SENSOR PART
#define sensor A0                 // SOIL SENSOR OUTPUT PART
#define trig D5                   // ULTRASOUND_TRIGGER PART
#define echo D6                   // ULTRASOUND_ECHO PART
#define relay D4
//==============================================================END PART=============================================================================
//==========================================================VARIABLE PIN PART========================================================================
const int dry_condition = 600;    // value for dry sensor from calibration
const int wet_condition = 280;    // value for wet sensor from calibration
int readSoil;
int MapSoilSensor;

DHT dht(DHTPIN, DHTTYPE);         // Initialize DHT sensor, DHT dht(Pin_used, Type_of_DHT_Sensor)
//==============================================================END PART=============================================================================
//========================================================WIFI CONNECTION PART=======================================================================
char auth[] = BLYNK_AUTH_TOKEN;   //Enter your Auth token
//SSID and Password of WiFi router.

//phone
//char ssid[] = "Rn135";          // WIFI name
//char pass[] = "IoTprj01";       //WIFI password

//router
char ssid[] = "HomeNet_2.4G";     // WIFI name
char pass[] = "88888888";         // WIFI password


//Host & httpsPort
const char* host = "script.google.com";
const int httpsPort = 443;

WiFiClientSecure client;          // Create a WiFiClientSecure object.

//spreadsheet script ID
String GAS_ID = "AKfycbzaOrwnXOo8Ox6QgdH_EaQoGZjkW2RLSDJsjhK4VkKIVaTtSMzV1GdNJjmxUAtU8uKQ"; 
//==============================================================END PART=============================================================================
//============================================================VOID SETUP=============================================================================
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(500);
  pinMode(soil,OUTPUT);
  pinMode(sensor,INPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  dht.begin();                     //Start reading DHT11 sensors
  delay(500);

  WiFi.begin(ssid, pass);          //Connect to your WiFi router
  Serial.println("");
  
  //Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
  }
  //----------------------------------------
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Blynk.begin(auth, ssid, pass);
  client.setInsecure();
}
//==============================================================END PART=============================================================================
//============================================================VOID LOOP==============================================================================
void loop() {
  Blynk.run();
  Serial.println("\n8266 awake. Begin checking..");
//===================================================TEMPERATURE & HUMIDITY PART===================================================================== 
  // Read temperature and humidity data from the sensor
  float temperature = dht.readTemperature(); // Read temperature in Celsius
  float humidity = dht.readHumidity();       // Read humidity as a percentage
  delay(2000); // Delay for 2 seconds before taking the next reading
  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature) && isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor !");
    delay(500);
    return;
  }
  //print message to serial
  String PrintTemperature = "Temperature : " + String(temperature) + " °C \n";
  String PrintHumidity = "Humidity : " + String(humidity) + " % \n";
//================================================END OF TEMPERATURE & HUMIDITY PART=================================================================
  delay(500);
//========================================================SOIL MOISTURE PART=========================================================================
  digitalWrite(soil, HIGH);
  delay(100);
  readSoil = analogRead(sensor);       
  digitalWrite(soil, LOW);
  delay(100);
  MapSoilSensor=map(readSoil, wet_condition, dry_condition, 100, 0);  
  //print message to serial
  String PrintSoilMoistureCapture = "Soil value : " + String(readSoil) + "\n";
  String PrintSoilMoistureCapturePercentage = "Soil moisture percentage : " + String(MapSoilSensor) + " % \n";
//=====================================================END OF SOIL MOISTURE PART=====================================================================
delay(500);
//==========================================================WATER LEVEL PART=========================================================================  
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long t = pulseIn(echo, HIGH);
  long cm = t / 29 / 2;
  int depth = 17;
  
  Serial.println(cm);
  long level= depth-cm;
  if (level<0)
  level=0;
  level = map(level,0,depth-3,0,100);
  //print message to serial
  String PrintWaterLevel = "Water Percentage : " + String(level) + " % \n";
//======================================================END OF WATER LEVEL PART======================================================================
delay(500);
//=======================================================CAPTURE ALL DATA PART=======================================================================
  Serial.println(PrintSoilMoistureCapture);
  Serial.println(PrintTemperature);
  Serial.println(PrintHumidity);
  Serial.println(PrintWaterLevel);

  Blynk.virtualWrite(V0, MapSoilSensor);
  Blynk.virtualWrite(V2, temperature);
  Blynk.virtualWrite(V3 , humidity);
  Blynk.virtualWrite(V4, level);
//====================================================END OF CAPTURE ALL DATA PART===================================================================

//=====================================================CONNECT TO GOOGLE HOST PART===================================================================
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
//==================================================END OF CONNECT TO GOOGLE HOST PART===============================================================

//============================================PROCESSING DATA AND SENDING DATA TO GOOGLE SHEET PART==================================================
  String string_temperature =  String(temperature);
  // String string_temperature =  String(tem, DEC); 
  String string_humidity =  String(humidity); 
  String string_soil =String(MapSoilSensor);
  String string_water =String(level);
  //	Water_Level		Humidity
  String url = "/macros/s/" + GAS_ID + "/exec?Soil_Sensor="+string_soil+"&Water_Level="+string_water+"&Temperature="+string_temperature+"&Humidity="+string_humidity;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  //----------------------------------------

  //----------------------------------------Checking whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
//========================================END OF PROCESSING DATA AND SENDING DATA TO GOOGLE SHEET PART===============================================
}
//==============================================================END PART=============================================================================

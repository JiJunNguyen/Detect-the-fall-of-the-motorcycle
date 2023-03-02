#include <Wire.h>
#include <MPU6050.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

#define buzzer 4

#define RXPin 3
#define TXPin 2

TinyGPSPlus gps;
SoftwareSerial SerialGPS(RXPin, TXPin);
SoftwareSerial SerialGSM(6,7);

void getGPS();
void updateGPS();
void sendsms();
float lat, lon;
int addr_lat = 0;
int addr_lon = 512;

MPU6050 mpu;

const double pi = 3.14159265;
int16_t Accel_X_RAW, Accel_Y_RAW, Accel_Z_RAW, Gyro_X_RAW, Gyro_Y_RAW, Gyro_Z_RAW;
double Ax,Ay,Az;
double Gx,Gy,Gz;
double Asum, Gsum;
double angle_x, angle_y;
boolean check1=false, check2=false, check3 = false, check4 = false;
byte timer = 0, timer1 = 0;


void setup(){   
 pinMode(buzzer, OUTPUT);
 Wire.begin();
 SerialGSM.begin(9600);
 Serial.begin(9600);
 SerialGPS.begin(9600);
 mpu.initialize();   
}
 
void loop(){
      
      updateGPS();
      
      mpu.getMotion6(&Accel_X_RAW, &Accel_Y_RAW, &Accel_Z_RAW, &Gyro_X_RAW, &Gyro_Y_RAW, &Gyro_Z_RAW);
      
      Gx = Gyro_X_RAW/131.0;
      Gy = Gyro_Y_RAW/131.0;
      Gz = Gyro_Z_RAW/131.0;
   
      Gsum = sqrt(pow(Gx,2) + pow(Gy,2) + pow(Gz,2));
      
      if (check3==true || check4==true){
            getGPS();
            sendsms();
            for (int i=0; i <=3; i++) {   
               digitalWrite(buzzer, HIGH);
               delay(1000);
               digitalWrite(buzzer, LOW);
               delay(200);
            }
            delay(1000);
            Serial.println("Message Sent!");
            check3=false;
            check4=false;
        }

       
     if (timer >= 11){
       Ax = Accel_X_RAW/16384.0;
       Ay = Accel_Y_RAW/16384.0;
       Az = Accel_Z_RAW/16384.0;
       angle_x = atan(Ay/(sqrt(pow(Ax,2) + pow(Az,2)))) * 180 / pi;
       if (abs(angle_x) > 60 || abs(Ay) >= 0.7){
            check2=true;
            Serial.println("Accident");
            Serial.println(Gsum);          
            check1=false;
            timer = 0; 
           }
         else {
            timer=0;
            check1=false;
            Serial.println("No fall");
         }
       }
      if (timer1 >= 31){
          Ax = Accel_X_RAW/16384.0;
          Ay = Accel_Y_RAW/16384.0;
          Az = Accel_Z_RAW/16384.0;

          angle_x = atan(Ay/(sqrt(pow(Ax,2) + pow(Az,2)))) * 180 / pi;       
          if (abs(angle_x) > 60 || abs(Ay) >= 0.7){
              check4=true;
              Serial.println("Accident");          
              check2=false;
              timer1 = 0;
           }
           else{
              timer1 = 0;
              check2 = false; 
              Serial.println("No fall");
           }
       }
           
     if (check1==true){
         timer++;   
     }
     
     if (check2==true){
         timer1++;
     }
     
     if (Gsum >= 73 && check1==false){ 
           check1=true;
           Serial.println("Angular velocty exceeds");
     }
     else if (Gsum > 0 && Gsum <73){    
           check2=true;
           Serial.println("Maybe Fall");
     }
     delay(100);

}    

void sendsms(){
   getGPS();    
   SerialGSM.println("AT+CMGF=1");   
   delay(500);     
   
   SerialGSM.print("AT+CMGS=\"+84968480461\"\r");
   delay(500);
   
   SerialGSM.println("Alerting! Mr. Trung is having an accident. The location is:");
   SerialGSM.print("http://maps.google.com/?q=loc:");
   SerialGSM.print(lat, 6);
   SerialGSM.print(",");
   SerialGSM.println(lon, 6);
   delay(300);
   SerialGSM.write(byte(26));
   delay(1000);  
}

void updateGPS(){
  while (SerialGPS.available() > 0)
    if (gps.encode(SerialGPS.read()))
      getGPS();
}

void getGPS(){
  if (gps.location.isValid())
  {
    lat = gps.location.lat();
    lon = gps.location.lng();
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    if (lat != 0.00 & lon != 0.00){
      EEPROM.put(addr_lat, lat);
      delay(100);
      EEPROM.put(addr_lon, lon);
      delay(100);
    }
  }
  else 
  {
    Serial.println("Location is not available");
    //Update the latest location
    EEPROM.get( addr_lat, lat );
    EEPROM.get( addr_lon, lon );
    Serial.println("The lately location: ");
    Serial.print(lat, 6);
    Serial.print(", ");
    Serial.println(lon, 6);
  }
  Serial.println();
}

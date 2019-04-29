#include <M5StickC.h>
#include <NMEAGPS.h>
#include <stdio.h>
#include <string>
#define PI 3.14159265
#define MAG 38
#define MAGvector 10
HardwareSerial SerialGPS(2);
typedef struct GPGSV
{
  int satnumber;
  int elevation;
  int azimuth;
  int cn;
};
typedef struct GPRMC
{
  int h;
  int m;
  int s;
  String stat;
  float latitude;
  String nors;
  float longitude;
  String eorw;
  float velocity;
  float degree;
  int d;
  int mo;
  int y;
};
String term;
int totalsentence;
String numberofsatsstring;
short numberofsats;
char test;
GPGSV gpgsv[20];
GPRMC gprmc;
int counter = 0;
void setup()
{
  M5.begin();
  M5.Lcd.setRotation(0);
  SerialGPS.begin(9600, SERIAL_8N1, 36, 36);
  // put your setup code here, to run once:
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.drawCircle(40, 40, 38, BLUE);
  M5.Lcd.drawLine(0, 40, 80, 40, BLUE);
  M5.Lcd.drawLine(40, 0, 40, 80, BLUE);
}
void refresh()
{
}
void loop()
{
  String sentenceType = "";
  term = "";
  while (SerialGPS.available() > 0)
  {
    Serial.println("counter=" + String(counter));
    counter++;
    if (counter >= 40)
    {
      counter = 0;
      M5.Lcd.fillRect(0, 0, 80, 80, BLACK);
    }
    M5.Lcd.drawCircle(40, 40, 38, BLUE);
    M5.Lcd.drawLine(0, 40, 80, 40, BLUE);
    M5.Lcd.drawLine(40, 0, 40, 80, BLUE);
    M5.Lcd.drawLine(40, 40, 40 + MAGvector * sin(gprmc.degree / (180 / PI)), 40 - 1 * MAGvector * cos(gprmc.degree / (180 / PI)), GREEN);
    for (int i = 0; i < 20; i++)
    {
      if (gpgsv[i].cn != 0 && gpgsv[i].satnumber != 0)
      {
        M5.Lcd.fillCircle(sin((gpgsv[i].azimuth) / (180 / PI)) * cos(gpgsv[i].elevation / (180 / PI)) * MAG + 40, -1 * cos((gpgsv[i].azimuth) / (180 / PI)) * cos(gpgsv[i].elevation / (180 / PI)) * MAG + 40, 1, RED);
      }
      else if (gpgsv[i].cn == 0 && gpgsv[i].satnumber != 0)
      {
        Serial.println("refreshed");
        M5.Lcd.fillCircle(sin((gpgsv[i].azimuth) / (180 / PI)) * cos(gpgsv[i].elevation / (180 / PI)) * MAG + 40, -1 * cos((gpgsv[i].azimuth) / (180 / PI)) * cos(gpgsv[i].elevation / (180 / PI)) * MAG + 40, 1, WHITE);
      }
      gpgsv[i].satnumber = gpgsv[i].elevation = gpgsv[i].azimuth = gpgsv[i].cn = 0;
    }
    term = SerialGPS.readStringUntil(',');
    Serial.println("term: " + term);
    if (term == "$GPGSV")
    {
      int numberofsentences = SerialGPS.readStringUntil(',').toInt();
      int sentencenumber = SerialGPS.readStringUntil(',').toInt();
      int numberofsats = SerialGPS.readStringUntil(',').toInt();

      int number;
      if (numberofsats - (4 * (sentencenumber - 1)) >= 4)
      {
        number = 4;
      }
      else
      {
        number = numberofsats - (4 * (sentencenumber - 1));
      }
      Serial.println(number);
      Serial.println(numberofsats);
      for (int i = 0; i < number; i++)
      {
        gpgsv[(numberofsentences - 1) * 4 + i].satnumber = SerialGPS.readStringUntil(',').toInt();
        gpgsv[(numberofsentences - 1) * 4 + i].elevation = SerialGPS.readStringUntil(',').toInt();
        gpgsv[(numberofsentences - 1) * 4 + i].azimuth = SerialGPS.readStringUntil(',').toInt();
        if (i + 1 == numberofsats - (4 * (sentencenumber - 1)) || i == 3)
        {
          gpgsv[(numberofsentences - 1) * 4 + i].cn = SerialGPS.readStringUntil('*').toInt();
        }
        else
        {
          gpgsv[(numberofsentences - 1) * 4 + i].cn = SerialGPS.readStringUntil(',').toInt();
        }
        Serial.printf("%d,%d,%d,%d\n", gpgsv[(numberofsentences - 1) * 4 + i].satnumber, gpgsv[(numberofsentences - 1) * 4 + i].elevation, gpgsv[(numberofsentences - 1) * 4 + i].azimuth, gpgsv[(numberofsentences - 1) * 4 + i].cn);
      }
      Serial.println("void: " + SerialGPS.readStringUntil('\n'));
      if (sentencenumber == 4)
      {
      }
    }
    else if (term == "$GPRMC")
    {
      int temp = SerialGPS.readStringUntil('.').toInt();
      float tempdouble;
      gprmc.s = temp % 100;
      gprmc.m = (temp / 100) % 100;
      gprmc.h = (temp / 10000) % 100;
      SerialGPS.readStringUntil(',');
      gprmc.stat = SerialGPS.readStringUntil(',');
      tempdouble = SerialGPS.readStringUntil(',').toFloat();
      gprmc.latitude = floorf(tempdouble / 100);
      gprmc.latitude += fmod(tempdouble, 100) / 60;
      gprmc.nors = SerialGPS.readStringUntil(',');
      tempdouble = SerialGPS.readStringUntil(',').toFloat();
      gprmc.longitude = floorf(tempdouble / 100);
      gprmc.longitude += fmod(tempdouble, 100) / 60;
      gprmc.eorw = SerialGPS.readStringUntil(',');
      gprmc.velocity = SerialGPS.readStringUntil(',').toFloat();
      gprmc.degree = SerialGPS.readStringUntil(',').toFloat();
      temp = SerialGPS.readStringUntil(',').toInt();
      gprmc.y = temp % 100;
      gprmc.mo = (temp / 100) % 100;
      gprmc.d = (temp / 10000) % 100;
      //SerialGPS.readStringUntil(',');
      //SerialGPS.readStringUntil(',');
      //SerialGPS.readStringUntil(',');
      SerialGPS.readStringUntil('\n');
      char tempstr[20] = {'\0'};
      sprintf(tempstr, "20%d/%d/%d ", gprmc.y, gprmc.mo, gprmc.d);
      M5.Lcd.drawString(tempstr, 0, 80);
      sprintf(tempstr, "%d:%d:%d      ", gprmc.h, gprmc.m, gprmc.s);
      M5.Lcd.drawString(tempstr, 0, 90);
      sprintf(tempstr, "%08.6fN       ", gprmc.latitude);
      M5.Lcd.drawString(tempstr, 0, 100);
      sprintf(tempstr, "%08.6fE       ", gprmc.longitude);
      M5.Lcd.drawString(tempstr, 0, 110);
      sprintf(tempstr, "v=%f     ", gprmc.velocity);
      M5.Lcd.drawString(tempstr, 0, 120);
      sprintf(tempstr, "azi=%06.3f     ", gprmc.degree);
      M5.Lcd.drawString(tempstr, 0, 130);
      //M5.Lcd.drawString(term, 0, 5);
    }
    else
    {
      Serial.println("trash: " + SerialGPS.readStringUntil('\n'));
    }
  }
}

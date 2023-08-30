// 17/Jun:
// Update : Nhay led o che dong thu cong
//Noi dung Hien thi LCD
// Tang thoi gian gui du lieu giua 2 string => Firebase
// Muc uu tien cua cam bien mua la cao nhat => nut nhan => hen gio
// Kiem tra dieu kien thoi gian => Kiem tra dieu kien nhiet do

//24/Jun
//Update Ini Process

//29/Jun
//Update Calculate average data ( Only a Day)

//30/Jul
//Update average, modify send data to Thingspeak

//7-12-2020 : Update code use DHT11
//			Change App
#include <FirebaseArduino.h> // Gui du lieu len Firebase
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ThingSpeak.h> // Gui du lieu led web thingspeak
#include <WiFiUdp.h> // LIB use for realtime
#include <NTPClient.h> // LIB use for realtime
//#include <SHT1x.h> // Sensor SHT10
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h" 
#define DHTTYPE DHT11
LiquidCrystal_I2C lcd(0x27,16,2);
// SCL -D1, SDA-D2
#define Led_test D0
#define BT1 D3
#define BT2 D5
#define BT3 D6
#define Pump_Machine D8
#define dataPin  D6
#define clockPin D7
#define Rain_Sensor A0
#define rain_value 315
#define dht_dpin 2 // D4


DHT dht(dht_dpin, DHTTYPE);
//#define WIFI_SSID "Xu-Xu" // change your WiFi name
//#define WIFI_PASSWORD "nguyenhang6868"  //change your Password  WiFi
#define WIFI_SSID "207" // change your WiFi name
#define WIFI_PASSWORD "207207207"  //change your Password  WiFi


const char *server = "api.thingspeak.com";
unsigned long myChannelNumber = 1236409;
const char * myWriteAPIKey = "NZLCGEGOCP6TD5AB";


//SHT1x sht1x(dataPin, clockPin);
float temp_c,old_temp = 0 ;
float humidity, old_humi = 0 ;
unsigned long time_check = 0,time_send_data = 0 ;
WiFiClient client;


//Declare
float temp_point = 0 ,humi_point = 0 ;
signed int hrs_point1,minu_point1;
signed int hrs_point2,minu_point2;
signed int hrs_point3,minu_point3;
signed int time_start =0, time_finish = 0;
signed int time_turn_on = 2 ;
signed int check_box1=0,check_box2=0,check_box3=0;

String real_time;
signed int sec,minu,hrs;
long time_process = 0;
int display_time=0,process_control_sendata= 0,process_control = 0 ;
boolean flag_turn_on_pump = 0;
boolean flag_send_humi = 0, flag_send_temp = 0, flag_send_data = 0,old_flag_pump = 0;
boolean flag_follow_condition = 0;
int flag_check_rain_ss = 2,old_flag_rain = 5, flag_check_time = 0 ;
String convert_send;
String data;
unsigned long count_average = 0;
float AverageHumi_Day = 0 ,AverageHumi_Month = 0 ,AverageTemp_Day = 0 ,AverageTemp_Month = 0 ;
unsigned long SumHumi_Day = 0, SumHumi_Month = 0, SumTemp_Day =0, SumTemp_Month = 0;
boolean flag_send_average = 0 ;
int process_control_send_average = 0;

WiFiUDP u;
NTPClient n(u,"3.vn.pool.ntp.org",7*3600); 

void blink_led(byte number);
void display_actual();
void display_setpoint();
void convert_time_h_m(String get_time,int *_hour,int *_minute);
void get_real_time(int _timeout,int *_hour,int *_minute,int *_sec);
void get_firebase();
void control_pump();
void check_conditions_on_pump();
void check_conditions_off_pump();
void check_rain_sensor();
void calculator_average_data();
void ini_process();
void auto_process();
void manual_process();

void setup() {
  Serial.begin(9600);
  pinMode(Led_test, OUTPUT);
  pinMode(Pump_Machine, OUTPUT);
  pinMode(BT1, INPUT_PULLUP);
  pinMode(BT2, INPUT_PULLUP);
  pinMode(BT3, INPUT_PULLUP);

  //lcd.init(); // Nếu sử dụng khác win 10 thì dùng khai báo này
lcd.begin(); // Nếu sử dụng win 10 thì dùng khai báo này
  lcd.backlight();
  lcd.print("Hello world    ");
  dht.begin();
  if (digitalRead(BT3)== 0)
  {
	  process_control = 5; // go to Manual Mode
	  digitalWrite(Led_test,1);
  }
  else
  {
	    // connect to wifi.
   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED)
		{
			lcd.setCursor(0,1);
			lcd.print("Connecting Wifi       ");
			Serial.print(".");
			delay(500);
			process_control++;
			if (process_control == 15)
			{
				break;
			}
		}
  if ( process_control <15)
  {
		process_control = 0;
		lcd.setCursor(0,1);
		lcd.print("Connected           ");
		Serial.println();
		Serial.print("connected: ");
		Serial.println(WiFi.localIP());
		//Firebase.begin("humi-temp.firebaseio.com"); // thay bằng địa chỉ ứng dụng của bạn
		Firebase.begin("testdht11-d75e1-default-rtdb.firebaseio.com" ,"mzgTqwSZEy5pJgnlQNeB3sqy8wQPzyhRgIcqiJfU");
		Firebase.stream("/GetData");
		ThingSpeak.begin(client);
		n.begin();
		n.update();
		real_time= n.getFormattedTime();
		blink_led(4);
  }
  else
  {
  lcd.setCursor(0,0);
  lcd.print("Can not connect");
    lcd.setCursor(0,1);
  lcd.print("internet !!!        ");
  
	digitalWrite(Led_test,1);
 delay(2000);
	lcd.clear();
	process_control = 5;	 
  }	  
  }
}


void loop() 
{
  
	switch (process_control)
	{
		case 0:
			ini_process();
			break;
		case 4:
			auto_process();
			break;
		case 5:
			manual_process();
			break;
	}
}


void blink_led(byte number)
{
  for (int i = 0 ; i<number;i++ )
  {
    digitalWrite(Led_test,1);
    delay(200);
    digitalWrite(Led_test,0);
    delay(200);
  }
}
void display_actual()
{
  lcd.setCursor(0,0);
  lcd.print("Actual:"); 
  lcd.print(hrs);
  lcd.print(":");
  lcd.print(minu);
  lcd.print("        ");

  lcd.setCursor(0,1);
  //lcd.print(round(temp_c));
  lcd.print("T=");
  lcd.print((int)temp_c);
  lcd.print("*C-");
  lcd.print("H=");
  //lcd.print(round(humidity));
  lcd.print((int)humidity);
  lcd.print("%      ");
}

void display_setpoint()
{
	if (check_box1 == 1)
	{
		lcd.setCursor(0,0);
		lcd.print("SetTime:"); 
		lcd.print(hrs_point1);
		lcd.print(":");
		lcd.print(minu_point1);
		lcd.print("     ");

	}
	else
	{
		if (check_box2 == 1)
		{
			lcd.setCursor(0,0);
			lcd.print("SetTime:"); 
			lcd.print(hrs_point2);
			lcd.print(":");
			lcd.print(minu_point2);
			lcd.print("     ");

		}
		else
		{
			if (check_box3 == 1)
			{
				lcd.setCursor(0,0);
				lcd.print("SetTime:"); 
				lcd.print(hrs_point3);
				lcd.print(":");
				lcd.print(minu_point3);
				lcd.print("     ");

			}
			else
			{
				lcd.setCursor(0,0);
				lcd.print("!! Not set Time       "); 			
			}
		}
	}
	
	lcd.setCursor(0,1);
	lcd.print("Set:T=");
	lcd.print((int)temp_point);
	lcd.print("*C-");
	lcd.print("H=");
	lcd.print((int)humi_point);
	lcd.print("%      ");  
}

void convert_time_h_m(String get_time,int *_hour,int *_minute)
{
	byte moc[3];byte count_moc=0;
	String chuoi1, chuoi2;
	get_time.remove(0,1); 
	get_time.remove((get_time.length()-1),1); 
//Serial.print(get_time );
    //Data is string, type: 12:20= => Convert to int
    for (int i = 0 ; i<get_time.length();i++)
  {
    if (get_time.charAt(i) == ':')
    {
      moc[count_moc] = i;
      count_moc++;
    }  
  }
  chuoi1 = get_time; 
  chuoi2 = get_time; 
  chuoi1.remove(moc[0]); // Ham remove : http://arduino.vn/reference/library/string/1/huong-dan-ham/remove
  chuoi2.remove(0,moc[0]+1);
  *_hour = chuoi1.toInt();
  *_minute = chuoi2.toInt();
  //Serial.print(*_hour );Serial.print("-" );Serial.println(*_minute );    
}
void get_real_time(int _timeout,int *_hour,int *_minute,int *_sec)
{
   if (millis()- time_process > _timeout)
  {
	time_process = millis();
	n.update();
	real_time= n.getFormattedTime();  // Get time from Internet
	byte moc[3];byte count_moc=0;
	String chuoi1, chuoi2,chuoi3;

	//Data is string, type: 12:20:35 => Convert to int
	for (int i = 0 ; i< real_time.length();i++)
	{
		if (real_time.charAt(i) == ':')
		{
			moc[count_moc] = i;
			count_moc++;
		}  
	}
	chuoi1 = real_time;
	chuoi2 = real_time;
	chuoi3 = real_time;
	chuoi1.remove(moc[0]); // Ham remove : http://arduino.vn/reference/library/string/1/huong-dan-ham/remove
	chuoi2.remove(0,moc[0]+1);
	chuoi3.remove(0,moc[1]+1);
	*_hour = chuoi1.toInt();
	*_minute = chuoi2.toInt();
	*_sec = chuoi3.toInt();
  }
}
void get_firebase() // Lấy dữ liệu từ Firebase
{ 
  if (Firebase.available()) // Nếu fire có sự thay đổi dữ liệu 		
  {
    FirebaseObject event = Firebase.readEvent();
    String eventType = event.getString("type");
    eventType.toLowerCase();
    if (eventType == "put") 
      {
        String path = event.getString("path");
        data = event.getString("data");
        if (path == "/pump")
        {
          if (data == "0")
          {
		      flag_turn_on_pump = 0 ;
        // Serial.println("Da nhan du lieu tu FireBase - OFF May Bom");
          }
          if (data == "1")
          {
		       flag_turn_on_pump = 1;
        //  Serial.println("Da nhan du lieu tu FireBase - ON May Bom");
          }
        }
		if (path == "/SetHumi")
        {
          data.remove(0,1); 
          data.remove((data.length()-1),1);
		  humi_point =  data.toFloat();         
        }
		if (path == "/SetTemp")
        {
          data.remove(0,1); 
          data.remove((data.length()-1),1);
		  temp_point =  data.toFloat();         
        }
		if (path == "/SetTime1")
        {
			convert_time_h_m(data,&hrs_point1,&minu_point1);      
        }	
		if (path == "/SetTime2")
        {
			convert_time_h_m(data,&hrs_point2,&minu_point2);      
        }
		if (path == "/SetTime3")
        {
			convert_time_h_m(data,&hrs_point3,&minu_point3);      
        }
		
		if (path == "/CheckBox1")
        {
			data.remove(0,1); 
			data.remove((data.length()-1),1); 
			check_box1 = data.toInt();			
        }
		if (path == "/CheckBox2")
        {
			data.remove(0,1); 
			data.remove((data.length()-1),1); 
			check_box2 = data.toInt();			
        }
		if (path == "/CheckBox3")
        {
			data.remove(0,1); 
			data.remove((data.length()-1),1); 
			check_box3 = data.toInt();			
        }		
			
		
      }
  }
}
void control_pump()
{
	if (old_flag_pump == flag_turn_on_pump)
	{}
	else
	{
		if (flag_check_rain_ss ==1)
		{
			flag_turn_on_pump =0; // Neu dang mua, thi luon luon tat may bom
		}
		old_flag_pump = flag_turn_on_pump;
		convert_send = String(old_flag_pump);
	//	Serial.println("Bat May Bom");
		Firebase.setString("/PumpStatus", convert_send);
		delay(50);
		Firebase.setString("/PumpStatus", convert_send);
		delay(50);
		Firebase.setString("/PumpStatus", convert_send);
		if (old_flag_pump == 1 && flag_check_rain_ss ==0)
		{
			digitalWrite(Pump_Machine,HIGH);
			digitalWrite(Led_test,HIGH); 			
		}
			
		if (old_flag_pump == 0)
		{
			digitalWrite(Pump_Machine,LOW);
			digitalWrite(Led_test,LOW);
		}		
	}
	
}
void check_conditions_on_pump()
{
	if (check_box1 == 1 )
	{
		if (hrs == hrs_point1 && minu == minu_point1 && sec <10)
		{
			flag_check_time = 1;
		}
	}
	if (check_box2 == 1 )
	{
		if (hrs == hrs_point2 && minu == minu_point2 && sec <10)
		{
			flag_check_time = 1;
		}
	}
	if (check_box3 == 1 )
	{
		if (hrs == hrs_point3 && minu == minu_point3 && sec <10)
		{
			flag_check_time = 1;
		}
	}
	
	if (flag_check_time == 1)
	{
		if (temp_c >temp_point || humidity < humi_point)
		{
    Firebase.setString("/GetData/pump", "1");   
			flag_turn_on_pump = 1;
			time_start = minu;
			time_finish = time_start + time_turn_on;
			if (time_finish >=60)time_finish = time_finish - 60;
			flag_follow_condition = 1;			
		}	
		flag_check_time = 0;
	}

}
void check_conditions_off_pump()
{
	if (flag_follow_condition == 1)
	{
		if(minu == time_finish)
		{
      Firebase.setString("/GetData/pump", "0");   
			flag_turn_on_pump = 0;
			flag_follow_condition = 0;
		}
	}
}

void check_rain_sensor()
{
  Serial.print("Rain Sensor");Serial.println(analogRead(Rain_Sensor));
	if(analogRead(Rain_Sensor)< rain_value) // Rain On
	{
		flag_check_rain_ss = 1;
		flag_turn_on_pump =0;
  // Serial.print("Troi Mua");
	}
	else
	{
		flag_check_rain_ss =0;
	}
	if (flag_check_rain_ss != old_flag_rain)
	{

	//	Serial.print("Rain Sensor");Serial.println(analogRead(Rain_Sensor));
		old_flag_rain = flag_check_rain_ss;
		convert_send = String(old_flag_rain);
		Firebase.setString("/RainSensor1", convert_send);			
	}
}
void calculator_average_data()
{
	SumTemp_Day = SumTemp_Day + temp_c;
	SumHumi_Day = SumHumi_Day + humidity;
	count_average ++;
	//if (hrs == 23 && minu == 55 && flag_send_average == 0 && sec < 25)
	if (hrs == hrs_point3 && minu == minu_point3 && flag_send_average == 0 && sec < 30)
	{		
		flag_send_average = 1;
		AverageTemp_Day =(float) SumTemp_Day / count_average;
		AverageHumi_Day =(float) SumHumi_Day / count_average;
	}
	if (flag_send_average == 1)
	{
		process_control_send_average++;
		switch(process_control_send_average)
			 {
			  case 1:
					ThingSpeak.writeField(myChannelNumber, 3, AverageTemp_Day, myWriteAPIKey);			  
					break;
			  case 2:
					Firebase.setFloat("/AverageTemp_Day", AverageTemp_Day);
					break;
			  case 3: 
					ThingSpeak.writeField(myChannelNumber, 4, AverageHumi_Day, myWriteAPIKey);
					 break;  			  
			  case 4: 
 					Firebase.setFloat("/AverageHumi_Day", AverageHumi_Day);               
					break;          
			 }
		if(process_control_send_average == 4)
		{
			process_control_send_average=0;  
			flag_send_average = 0;	
			SumTemp_Day = 0 ;
			SumHumi_Day = 0 ;
			count_average = 0 ;		
		}  	
	}
	
}
void ini_process() //0
{
	FirebaseObject fbo = Firebase.get("/GetData");
	lcd.setCursor(0,0);
	lcd.print("Waitting              "); 
	lcd.setCursor(0,1);
	lcd.print("Ini Process ....      "); 
	
	data = fbo.getString("CheckBox1");
	data.remove(0,1); 
	data.remove((data.length()-1),1); 
	check_box1 = data.toInt();
//	Serial.println(check_box1);
	delay(200);
	data = fbo.getString("CheckBox2");
	data.remove(0,1); 
	data.remove((data.length()-1),1); 
	check_box2 = data.toInt();
//	Serial.println(check_box2);
	delay(200);
	data = fbo.getString("CheckBox3");
	data.remove(0,1); 
	data.remove((data.length()-1),1); 
	check_box3 = data.toInt();
//	Serial.println(check_box3);
	
	delay(200);
	data = fbo.getString("SetHumi");
	data.remove(0,1); 
	data.remove((data.length()-1),1);
	humi_point =  data.toFloat();
//	Serial.println(humi_point);
	delay(200);
	data = fbo.getString("SetTemp");
	data.remove(0,1); 
	data.remove((data.length()-1),1);
	temp_point =  data.toFloat(); 
//	Serial.println(temp_point);	
	delay(200);
 
  data = fbo.getString("pump");
  if (data == "0")
    {
    flag_turn_on_pump = 0 ;
    }
    if (data == "1")
    {
    flag_turn_on_pump = 1;
    }
    delay(200);  
	data = fbo.getString("SetTime1");
	convert_time_h_m(data,&hrs_point1,&minu_point1); 
	delay(200);	
	data = fbo.getString("SetTime2");
	convert_time_h_m(data,&hrs_point2,&minu_point2);
	delay(200);
	data = fbo.getString("SetTime3");
	convert_time_h_m(data,&hrs_point3,&minu_point3);
	
	process_control = 4;
	
}
void auto_process() // 4
{
    get_real_time(10000,&hrs, &minu,&sec);
	if (millis() - time_check >8000)
	{
	  time_check = millis(); 
	//  temp_c = sht1x.readTemperatureC();
	  //humidity = sht1x.readHumidity();
    temp_c = dht.readTemperature();//sht1x.readTemperatureC();
    humidity = dht.readHumidity();//sht1x.readHumidity();
	  check_rain_sensor();
    if((temp_c<100) && (temp_c > -5) )
    {
    if (   (abs(temp_c-old_temp) > 0.5)    ||     (abs( humidity - old_humi) > 1.5)    )
    {
      flag_send_data = 1;
      old_temp = temp_c;
      old_humi = humidity;
    }
        if (flag_send_data == 1 )
      {
       process_control_sendata++;
      switch(process_control_sendata)
         {
          case 1:
            ThingSpeak.writeField(myChannelNumber, 2, humidity, myWriteAPIKey);       
            break;
          case 2:
            Firebase.setFloat("/ActualHumi1", humidity);
            break;
          case 3: 
            ThingSpeak.writeField(myChannelNumber, 1, temp_c, myWriteAPIKey);
             break;  
          case 4: 
            Firebase.setFloat("/ActualTemp1", temp_c);               
            break;                     
         }
      if(process_control_sendata == 4)
      {
        process_control_sendata=0;  
        flag_send_data = 0;       
      }  
      }
      calculator_average_data(); // Calculate and Send       
    }

	}
    get_firebase();
	
	// Hiển thị LCD ////
    display_time++;
    if (display_time<60) display_actual();
    if (display_time>=60)
    {
      display_setpoint();
      if (display_time>=120)display_time=0;
    }
	/////////////////////
	
	
	if (digitalRead(BT2) == 0)
	{
		flag_turn_on_pump = 1 ;
		Firebase.setString("/GetData/pump", "1");	

	}
	if (digitalRead(BT3) == 0 )
	{	
		flag_turn_on_pump = 0 ;	
		Firebase.setString("/GetData/pump", "0");			
	}
	check_conditions_on_pump();
	check_conditions_off_pump();
	control_pump();	
}

void manual_process()
{
  lcd.setCursor(0,0);
  lcd.print("Manual Mode"); 
  	if (millis() - time_check >2000)
	{
		time_check = millis(); 
		temp_c = dht.readTemperature();
		humidity = dht.readHumidity();
		lcd.setCursor(0,1);
		lcd.print((int)temp_c);
		lcd.print("*C-");
		lcd.print("H:");
		lcd.print((int)humidity);
		lcd.print("%      ");
		digitalWrite(Led_test,flag_turn_on_pump);
		flag_turn_on_pump = !flag_turn_on_pump;
	 
	}
  if (digitalRead(BT2) == 0)
  {	  
	  digitalWrite(Pump_Machine,HIGH);
  }
  if (digitalRead(BT3) == 0 )
  {
	  digitalWrite(Pump_Machine,LOW);  
  }	  
}

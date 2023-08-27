 #include "DHT.h" 
 #include <Wire.h> 
 #include <LiquidCrystal_I2C.h>
 LiquidCrystal_I2C lcd(0x27,16,2);
 
 const int DHTPIN = 2;       //Đọc dữ liệu từ DHT11 ở chân 2 trên mạch Arduino
 const int DHTTYPE = DHT11;  //Khai báo loại cảm biến, có 2 loại là DHT11 và DHT22
 DHT dht(DHTPIN, DHTTYPE);
void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  dht.begin(); // khoi dong cam bien 
   lcd.init();       //Khởi động màn hình. Bắt đầu cho phép Arduino sử dụng màn hình, cũng giống như dht.begin() trong chương trình trên
   lcd.backlight();   //Bật đèn nền
   //lcd.print("") ;
   //lcd.setCursor(0,1);
   
}

void loop() {
  // put your main code here, to run repeatedly:
     float h = dht.readHumidity();    //Đọc độ ẩm
     float t = dht.readTemperature(); //Đọc nhiệt độ
     float t1=(t*20)  ;
     //Serial.print("Nhiet do: ");
       //Serial.println(t1);               //Xuất nhiệt độ
       //Serial.print("Do am: ");
       //Serial.println(h);               //Xuất độ ẩm
  
       //Serial.println();                //Xuống hàng
       //delay(1000);//Đợi 1 giây
       lcd.print("nhiet do :");
       lcd.print(round(t1));
       lcd.setCursor (0, 1);
       lcd.print("do am : %");
       lcd.print(round(h));
      
}

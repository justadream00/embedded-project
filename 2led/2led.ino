int greenled = 8 ;
int blueled = 9 ;
void setup() {
  // put your setup code here, to run once:
pinMode(greenled,OUTPUT);
pinMode(blueled,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(greenled,HIGH);
digitalWrite(blueled,LOW);
delay(1000);
digitalWrite(greenled,LOW);
digitalWrite(blueled,HIGH);
delay(1000);
}

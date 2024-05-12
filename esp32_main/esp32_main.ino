#include <WiFi.h> 
#include <PubSubClient.h>
#include "Wire.h"
#include "MPU6050.h"

/* Nb Timer */
#define MAX_WAIT_FOR_TIMER 3

/* pin HW */
#define pin_btn 23
#define pin_lum 36

#define pin_SDA 4
#define pin_SCL 15

/* wifi  */
const char* ssid = "Livebox-4F30";
const char* password = "RfSYkMrmuptW3PRZEE";
WiFiClient espClient;

/* mqtt */
const char mqtt_broker[] = "192.168.1.62";
const char *topic = "topic";
const char *led_topic = "led";
const char *lum_topic = "lum";
const char *btn_topic = "btn";
const char *AX_topic = "angle_x";
const char *AY_topic = "angle_y";

const int mqtt_port = 1883;
PubSubClient client(espClient);

/*LED*/
const int ledPin = LED_BUILTIN;
int etat = 1; //LED ON

/* Variable bouton */
int btn = 0;
int btn_prev = 0;
int btn_flag = 0;

/* Variable photoresistance */
int lum = 0;
int lum_prev = 0;

/* Variable L'accéléromètre */
int16_t ax, ay, az; 
float r_ax, r_ay, r_az;

int angle_x, angle_y;
int angle_x_prev, angle_y_prev = 0;

MPU6050 mpu;

/* Structure pour la temporisation des tâches */
struct timing {
  int timer;             
  unsigned long period;
};

/* Les fonctions */
void init_wifi(){
  WiFi.mode(WIFI_STA); //Optional
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");

  while(WiFi.status() != WL_CONNECTED){
      Serial.print(".");
      delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void init_mqtt(){
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
      String client_id = "esp32-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
      if(client.connect(client_id.c_str())){
          Serial.println("MQTT broker connected\n");
      }else {
          Serial.print("failed with state");
          Serial.println(client.state());
          delay(2000);
      } 
      delay(2000);
  }

  // publish and subscribe
  client.publish(topic, "Hi, I'm ESP32 ^^");
  client.subscribe(led_topic);
}

/* Le TIMER */

unsigned int waitFor(int timer, unsigned long period){
  static unsigned long waitForTimer[MAX_WAIT_FOR_TIMER];  // il y a autant de timers que de tâches périodiques
  unsigned long newTime = micros() / period;              // numéro de la période modulo 2^32 
  int delta = newTime - waitForTimer[timer];              // delta entre la période courante et celle enregistrée
  if ( delta < 0 ) delta = 1 + newTime;                   // en cas de dépassement du nombre de périodes possibles sur 2^32 
  if ( delta ) waitForTimer[timer] = newTime;             // enregistrement du nouveau numéro de période
  return delta;
}

/* bouton */ 

struct timing btn_s;

void blink() { //Routine d'interuption
  if(!waitFor(btn_s.timer, btn_s.period)) return; 
  btn = digitalRead(pin_btn);
  if(btn != btn_prev){
    btn_prev = btn;
    if(btn_flag != 1) btn_flag = 1;
  }
}

void init_btn(struct timing* ctx, int timer, unsigned long period){
  ctx->timer = timer;
  ctx->period = period;
  pinMode(pin_btn, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin_btn), blink, CHANGE);
}

void loop_btn(){
  if(btn_flag == 1){
    btn_flag = 0;
    if(btn){
      client.publish(btn_topic, "0");
      Serial.println("bnt : Pas appuyé");
    }
    else{
      client.publish(btn_topic, "1");
      Serial.println("bnt : Appuyé");
    }
  }
}

/* LUMIERE */

struct timing lum_s;

void init_lum(struct timing* ctx, int timer, unsigned long period) {
  ctx->timer = timer;
  ctx->period = period;
}

void loop_lum(struct timing* ctx){
  if (!waitFor(ctx->timer, ctx->period)) return;
  
  lum = map(analogRead(pin_lum), 0, 4095, 100, 0);
  if(lum != lum_prev){
    lum_prev = lum;
    char msg_lum[3];
    itoa(lum, msg_lum, 10);
    Serial.print("Lum: ");
    Serial.println(msg_lum);
    client.publish(lum_topic, msg_lum);
  }
}

/* L'accéléromètre */

struct timing acc_s;

void init_acc(struct timing* ctx, int timer, unsigned long period){
  ctx->timer = timer;
  ctx->period = period;
  Wire.begin(pin_SDA,pin_SCL);
  mpu.initialize();
}

void loop_acc(struct timing* ctx){
  if (!waitFor(ctx->timer, ctx->period)) return;

  mpu.getAcceleration(&ax, &ay, &az);

  r_ax = map(ax, -32768, +32767, -200, 200);
  r_ay = map(ay, -32768, +32767, -200, 200);
  r_az = map(az, -32768, +32767, -200, 200);

  r_ax = r_ax/100 - 0.01;
  r_ay = r_ay/100 + 0.01;
  r_az = r_az/100 - 0.02;  

  angle_x = atan( r_ay/sqrt(r_ax*r_ax+r_az*r_az))/(3.1415/180);
  angle_y = atan(-r_ax/sqrt(r_ay*r_ay+r_az*r_az))/(3.1415/180);

  if((angle_x > angle_x_prev + 1) | (angle_x_prev-1 > angle_x)){
    angle_x_prev = angle_x;
    char msg_ax[4];
    if(angle_x < 0){
      angle_x = -1*angle_x;
      itoa(angle_x, msg_ax, 10);
      msg_ax[2] = msg_ax[1];
      msg_ax[1] = msg_ax[0];
      msg_ax[0] = '-';
    }
    else{
      itoa(angle_x, msg_ax, 10);
    }
    Serial.print("Angle X: ");
    Serial.println(msg_ax);
    client.publish(AX_topic, msg_ax);
  }

  if((angle_y > angle_y_prev + 1) | (angle_y_prev-1 > angle_y)){
    angle_y_prev = angle_y;
    char msg_ay[4];
    if(angle_y < 0){
      angle_y = -1*angle_y;
      itoa(angle_y, msg_ay, 10);
      msg_ay[2] = msg_ay[1];
      msg_ay[1] = msg_ay[0];
      msg_ay[0] = '-';
    }
    else{
      itoa(angle_y, msg_ay, 10);
    }
    Serial.print("Angle Y: ");
    Serial.println(angle_y);
    client.publish(AY_topic, msg_ay);
  }
}

/* Lecture message reçu */

void callback(char * topic, byte *message, unsigned int length)
{
  Serial.print("you have a message on topic:");
  Serial.println(topic);

  Serial.print( "message :");
  for(int i = 0; i<length ; i++){
    Serial.print( (char)message[i]);
  }

  Serial.println();

  char msg[length+1];
  memcpy(msg, message, length);
  msg[length] = '\0';

  if(strcmp(msg," ON") == 0){
    Serial.println("They want the leds on !");
    digitalWrite(ledPin, HIGH);
  }
  else if( strcmp(msg," OFF") == 0){
    Serial.println("They want the leds off !");
    digitalWrite(ledPin, LOW);
  }
}

void setup(){
  init_wifi();
  
  /*init led*/
  pinMode(ledPin, OUTPUT);

  /*init lum*/
  init_lum(&lum_s, 0, 500000);

  /*init btn*/
  init_btn(&btn_s, 1, 1000);

  /*init acc*/
  init_acc(&acc_s, 2, 100000);

  /* mqtt led */
  init_mqtt();

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  delay(1000);
}

void loop(){
  client.loop();
  loop_btn();
  loop_lum(&lum_s);
  loop_acc(&acc_s);
}
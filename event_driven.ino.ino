#define LED1       10
#define LED2       11
#define LED3       12
#define LED4       13
#define BUZZ        3
#define KEY1       A1
#define KEY2       A2
#define KEY3       A3
#define POT        A0

int ve_botao_1 = 0;
int ve_botao_2 = 0;
int ve_botao_3 = 0;

int estado_botao_1;
int estado_botao_2;
int estado_botao_3;

long timer = 0;
unsigned long last_millis;

void button_listen(int pin) {
  Serial.print("btlis");
  pinMode(pin, INPUT);
  if(pin == KEY1){
    ve_botao_1 = 1;
    estado_botao_1 = digitalRead(pin);
  }
  if(pin == KEY2){
    ve_botao_2 = 1;
    estado_botao_2 = digitalRead(pin);
  }
  if(pin == KEY3){
    ve_botao_3 = 1;
    estado_botao_3 = digitalRead(pin);
  }
}

void timer_set(int ms) {
  timer = ms;
  last_millis = millis();
}

/* Callbacks */
void inicio();
void button_changed(int pin, int v);
void time_expired();  

void setup () {
  Serial.begin(9600);
  inicio ();
}
void loop () {
  
  if(ve_botao_1 == 1){
    if(estado_botao_1 != digitalRead(KEY1)){
      estado_botao_1 = digitalRead(KEY1);
      button_changed(KEY1,estado_botao_1);
    }
  }
  if(ve_botao_2 == 1){
    if(estado_botao_2 != digitalRead(KEY2)){
      estado_botao_2 = digitalRead(KEY2);
      button_changed(KEY2,estado_botao_2);
    }
  }
  if(ve_botao_3 == 1){
    if(estado_botao_3 != digitalRead(KEY3)){
      estado_botao_3 = digitalRead(KEY3);
      button_changed(KEY3,estado_botao_3);
    }
  }
  
  if(timer > 0){
    unsigned long time = millis();
    timer -= (time - last_millis);
    last_millis = time;
    if(timer <= 0)
      time_expired();
  }
     
}

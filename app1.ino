#include "event_driven.h"

int estado_led_3 = 0;

void time_expired(){
  digitalWrite(LED3, estado_led_3);
  if(estado_led_3 == 0)
    estado_led_3 = 1;
  else if(estado_led_3 == 1)
    estado_led_3 = 0;
  timer_set(1000);
}

void button_changed (int pin, int v) {
  if(pin == KEY1)
    digitalWrite(LED1, v);
  if(pin == KEY2)
    digitalWrite(LED2, v);
}
void inicio () {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  button_listen(KEY1);
  button_listen(KEY2);
  timer_set(1000);
}

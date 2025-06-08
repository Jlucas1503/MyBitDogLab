#ifndef FUNCOES_GERAIS_H
#define FUNCOES_GERAIS_H

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "lwip/init.h"
#include "lwip/pbuf.h"
// ativar a biblioteca de LEDs WS2812B se necessário >> se atentar ao CMAKE >>
// #include "ws2818b.pio.h" // Biblioteca para controle do LED WS2812B

/* DEFINIÇÕES E CONSTANTES */
#define I2C_PORT    i2c1
#define PINO_SCL    14
#define PINO_SDA    15
#define BUZZER_PIN  21

// Para constantes usamos "static const" para ter ligação interna e evitar conflito
static const int LED_B = 12;
static const int LED_R = 13;
static const int LED_G = 11;
static const int SW = 22;

static const int BotaoA = 5;
static const int BotaoB = 6;
static const int eixoX  = 26;
static const int eixoY  = 27;
#define ADC_CHANNEL_0 0
#define ADC_CHANNEL_1 1

#define WIFI_SSID       ""
#define WIFI_PASS       ""
#define THINGSPEAK_HOST "api.thingspeak.com"
#define THINGSPEAK_PORT 80
#define API_KEY         ""
#define API_KEY_READ    ""
#define CHANNEL_ID      "2838418"

#define HTTP_RESPONSE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" \
                      "<!DOCTYPE html><html><body>" \
                      "<h1>Controle do LED</h1>" \
                      "<p><a href=\"/led/on\">Ligar LED</a></p>" \
                      "<p><a href=\"/led/off\">Desligar LED</a></p>" \
                      "</body></html>\r\n"

static const float DIVIDER_PWM = 16.0;
static const uint16_t PERIOD = 4096;

/* VARIÁVEIS GLOBAIS (DECLARADAS COM extern) */
extern volatile bool button_pressionado;
extern volatile bool connection_established;
extern uint pos_y;
extern ssd1306_t display;
extern struct tcp_pcb *tcp_client_pcb;
extern ip_addr_t server_ip;

/* Protótipos das funções do módulo de hardware */
void inicializacao(void);
void setup_pwm_led(uint led, uint *slice, uint16_t level);
void pwm_init_buzzer(uint pin);
void play_tone(uint pin, uint frequency, uint duration_ms);
void print_texto(char *msg, uint pos_x, uint pos_y, uint scale);
void print_retangulo(int x1, int x2, int y1, int y2);
void print_menu(uint posy);
void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value);

//led pwm
void led_pwm(int r, int g, int b);

/* Protótipos das funções de rede */
void dns_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg);



/* Protótipos para callbacks e timer */
void button_callback(uint gpio, uint32_t events);
int64_t timer_callback(alarm_id_t id, void *user_data);

#endif // FUNCOES_GERAIS_H
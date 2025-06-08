#include "funcoes_gerais.h"

/* DEFINIÇÃO DAS VARIÁVEIS GLOBAIS */
volatile bool button_pressionado = false;
volatile bool connection_established = false;
uint pos_y = 14;
ssd1306_t display;
struct tcp_pcb *tcp_client_pcb = NULL;
ip_addr_t server_ip;
uint16_t led_b_level = 100, led_r_level = 100;
uint slice_led_b, slice_led_r;

/*****************************************
 * FUNÇÕES DO MÓDULO DE HARDWARE
 *****************************************/
void setup_pwm_led(uint led, uint *slice, uint16_t level) {
    gpio_set_function(led, GPIO_FUNC_PWM);
    *slice = pwm_gpio_to_slice_num(led);
    pwm_set_clkdiv(*slice, DIVIDER_PWM);
    pwm_set_wrap(*slice, PERIOD);
    pwm_set_gpio_level(led, level);
    pwm_set_enabled(*slice, true);
}

void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f);
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(pin, 0);
}

void play_tone(uint pin, uint frequency, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t top = clock_freq / frequency - 1;
    pwm_set_wrap(slice_num, top);
    pwm_set_gpio_level(pin, top / 2);
    
    uint64_t start_time = time_us_64();
    while ((time_us_64() - start_time) < duration_ms * 1000) {
        if (button_pressionado)
            break;
        sleep_ms(1);
    }
    pwm_set_gpio_level(pin, 0);
    sleep_ms(50);
}

void print_texto(char *msg, uint pos_x, uint pos_y, uint scale) {
    ssd1306_draw_string(&display, pos_x, pos_y, scale, msg);
    ssd1306_show(&display);
}

void print_retangulo(int x1, int x2, int y1, int y2) {
    ssd1306_draw_empty_square(&display, x1, x2, y1, y2);
    ssd1306_show(&display);
}

void print_menu(uint posy) {
    ssd1306_clear(&display);
    print_texto("programa", 52, 2, 1);
    print_retangulo(2, posy, 120, 12);
    print_texto("funcao 1", 10, 18, 1);
    print_texto("funcao 2", 10, 30, 1);
    print_texto("funcao 3", 10, 42, 1);
}

void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value) {
    adc_select_input(ADC_CHANNEL_0);
    sleep_us(2);
    *vrx_value = adc_read();
    
    adc_select_input(ADC_CHANNEL_1);
    sleep_us(2);
    *vry_value = adc_read();
}

void inicializacao(void) {
    stdio_init_all();
    pwm_init_buzzer(BUZZER_PIN);
    
    adc_init();
    adc_gpio_init(eixoX);
    adc_gpio_init(eixoY);
    
    i2c_init(I2C_PORT, 600 * 1000);
    gpio_set_function(PINO_SCL, GPIO_FUNC_I2C);
    gpio_set_function(PINO_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(PINO_SCL);
    gpio_pull_up(PINO_SDA);
    display.external_vcc = false;
    ssd1306_init(&display, 128, 64, 0x3c, I2C_PORT);
    ssd1306_clear(&display);
    
    gpio_init(SW);
    gpio_set_dir(SW, GPIO_IN);
    gpio_pull_up(SW);
    
    gpio_init(LED_B);
    gpio_init(LED_R);
    gpio_init(LED_G);
    gpio_set_function(LED_B, GPIO_OUT);
    gpio_set_function(LED_R, GPIO_OUT);
    gpio_set_function(LED_G, GPIO_OUT);
    
    gpio_init(BotaoA);
    gpio_set_function(BotaoA, GPIO_IN);
    gpio_pull_up(BotaoA);
    gpio_init(BotaoB);
    gpio_set_function(BotaoB, GPIO_IN);
    gpio_pull_up(BotaoB);
    
    gpio_set_irq_enabled_with_callback(SW, GPIO_IRQ_EDGE_FALL, true, button_callback);

    uint slice_led_b, slice_led_r, slice_led_g;
    uint16_t led_b_level = 100, led_r_level = 100, led_g_level = 100;

    setup_pwm_led(LED_B, &slice_led_b, led_b_level);
    setup_pwm_led(LED_R, &slice_led_r, led_r_level);
    setup_pwm_led(LED_G, &slice_led_g, led_g_level);
}


// SETAR COR DO LED RGB ATRAVES DO PWM -- valores de 0 a 4096
void led_pwm(int r, int g, int b){
    pwm_set_gpio_level(LED_R, r);
    pwm_set_gpio_level(LED_G, g);
    pwm_set_gpio_level(LED_B, b);
}


/*****************************************
 * FUNÇÕES DE REDE (exemplos, conforme seu código original)
 *****************************************/
static err_t http_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        tcp_close(tpcb);
        return ERR_OK;
    }
    printf("Resposta do ThingSpeak: %.*s\n", p->len, (char *)p->payload);
    pbuf_free(p);
    return ERR_OK;
}

static err_t http_connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) {
        printf("Erro na conexão TCP\n");
        return err;
    }
    printf("Conectado ao ThingSpeak!\n");
    connection_established = true;
    return ERR_OK;
}

static err_t http_read_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        tcp_close(tpcb);
        return ERR_OK;
    }
    char *payload = (char *)p->payload;
    char *body = strstr(payload, "\r\n\r\n");
    if (body)
        body += 4;
    else
        body = payload;
    
}

void dns_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    if (ipaddr) {
        printf("Endereço IP: %s\n", ipaddr_ntoa(ipaddr));
        tcp_client_pcb = tcp_new();
        tcp_connect(tcp_client_pcb, ipaddr, THINGSPEAK_PORT, http_connected_callback);
    } else {
        printf("Falha na resolução DNS\n");
    }
}

/*****************************************
 * CALLBACKS E TIMER
 *****************************************/
void button_callback(uint gpio, uint32_t events) {
    static uint64_t last_press_time = 0;
    uint64_t current_time = time_us_64();
    if ((current_time - last_press_time) > 200000) {
        if (gpio == SW)
            button_pressionado = true;
        last_press_time = current_time;
    }
}

int64_t timer_callback(alarm_id_t id, void *user_data) {
    printf("Temporizador acionado!\n");
    return 1000000; // 1 segundo para o próximo
}
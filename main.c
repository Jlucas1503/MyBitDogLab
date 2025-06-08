#include "funcoes_gerais.h"

int main() {
    inicializacao();

    /* INICIALIZAÇÃO DA COMUNICAÇÃO POR HTTP >>>>>>>>>>>>>>>>>>>>>

    // Configura o temporizador
    add_alarm_in_ms(1000, timer_callback, NULL, true);

    // Configuração do Wi-Fi e demais funções devem ser chamadas conforme necessário
    if (cyw43_arch_init()) {
        printf("Falha ao iniciar Wi-Fi\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_blocking(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_MIXED_PSK)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        return 1;
    }
    printf("Wi-Fi conectado!\n");
    dns_gethostbyname(THINGSPEAK_HOST, &server_ip, dns_callback, NULL);      */


    led_pwm(0, 0, 0); // Desliga o LED RGB inicialmente;
    
    printf("Iniciando comunicação serial\n");
    
    /* AQUI MUDAMOS A QUANTIDADE E POSICAO DAS OPCOES DO MENU*/
    uint countDown = 0, countUp = 2, menu = 1, pos_y_anterior = 19;
    print_menu(pos_y);
    
    while (true) {
        uint adc_y_raw = adc_read();
        const uint bar_width = 40, adc_max = (1 << 12) - 1;
        uint bar_y_pos = adc_y_raw * bar_width / adc_max;
        if (bar_y_pos < 14 && countDown < 2) {
            pos_y += 12;
            countDown++;
            countUp--;
            menu++;
        } else if (bar_y_pos > 24 && countUp < 2) {
            pos_y -= 12;
            countDown--;
            countUp++;
            menu--;
        }
        sleep_ms(100);
        if (pos_y != pos_y_anterior)
            print_menu(pos_y);
        
        if (button_pressionado) {
            button_pressionado = false;
            switch(menu) {
                case 1:
                    ssd1306_clear(&display);
                    break;
                case 2:
                    ssd1306_clear(&display);
                    break;
                case 3:
                    ssd1306_clear(&display);
                    sleep_ms(1000); 
                    break;
            }
        }
        sleep_ms(100);
        pos_y_anterior = pos_y;
    }
    
    return 0;
}
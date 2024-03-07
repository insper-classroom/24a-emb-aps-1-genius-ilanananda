

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

const int LED_PIN_R = 18;
const int LED_PIN_B = 19;
const int LED_PIN_Y = 20;
const int LED_PIN_G = 21;

const int BTN_PIN_Y = 2;
const int BTN_PIN_R = 3;
const int BTN_PIN_B = 4;
const int BTN_PIN_G = 5;

const int BTN_PIN_START = 14;

const int BUZZER_PIN = 16;

const int BTN_MODE_RED = 8;
const int BTN_MODE_BLUE = 15;

volatile bool btn_y_state = false;
volatile bool btn_r_state = false;
volatile bool btn_b_state = false;
volatile bool btn_g_state = false;

volatile bool start = false;

volatile bool mode_black = false;

volatile bool mode_red = false;
volatile bool mode_blue = false;

volatile bool started = false;

void btn_callback(uint gpio, uint32_t events) {
    if(gpio == BTN_PIN_R && events == GPIO_IRQ_EDGE_FALL){
        btn_r_state = true;
    } else if (gpio == BTN_PIN_B && events == GPIO_IRQ_EDGE_FALL){
        btn_b_state = true;
    } else if(gpio == BTN_PIN_Y && events == GPIO_IRQ_EDGE_FALL){
        btn_y_state = true;
    } else if (gpio == BTN_PIN_G && events == GPIO_IRQ_EDGE_FALL){
        btn_g_state = true;
    } else if (gpio == BTN_PIN_START && events == GPIO_IRQ_EDGE_FALL){
        start = true;
        if (started) {
            mode_black = true;
        }
    } else if (gpio == BTN_MODE_RED && events == GPIO_IRQ_EDGE_FALL){
        mode_red = true;
    } else if (gpio == BTN_MODE_BLUE && events == GPIO_IRQ_EDGE_FALL){
        mode_blue = true;
    }
}

void inicializa_dispositivos(){

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);
    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_B);
    gpio_set_dir(BTN_PIN_B, GPIO_IN);
    gpio_pull_up(BTN_PIN_B);

    gpio_init(BTN_PIN_Y);
    gpio_set_dir(BTN_PIN_Y, GPIO_IN);
    gpio_pull_up(BTN_PIN_Y);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_init(BTN_PIN_START);
    gpio_set_dir(BTN_PIN_START, GPIO_IN);
    gpio_pull_up(BTN_PIN_START);

    gpio_init(BTN_MODE_RED);
    gpio_set_dir(BTN_MODE_RED, GPIO_IN);
    gpio_pull_up(BTN_MODE_RED);

    gpio_init(BTN_MODE_BLUE);
    gpio_set_dir(BTN_MODE_BLUE, GPIO_IN);
    gpio_pull_up(BTN_MODE_BLUE);

    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_B, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BTN_PIN_Y, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BTN_PIN_START, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BTN_MODE_RED, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BTN_MODE_BLUE, GPIO_IRQ_EDGE_FALL, true);
}

void play_sound(int indice) {
    int freqs[5] = {4000, 5000, 6000, 7000, 10000};
    int duration_ms = 10000;
    int t = 1000000/(freqs[indice]*2); // o 10^6 eh pq estamos convertendo para microsegundos e t = 1/f, e dividimos por 2 por ser metade da onda ligada e metade desligada
    int n = duration_ms / 10; // quantidade de vezes que fazemos a onda inteira
    for(int i = 0; i < n; i++){
        gpio_put(BUZZER_PIN, 1);
        sleep_us(t);
        gpio_put(BUZZER_PIN, 0);
        sleep_us(t);
    }
}

void play_wrong() {
    int freqs[4] = {440, 304, 261, 220};
    int duration_ms = 200;
    for (int i = 0; i < 4; i++) {
        int t = 1000000 / (freqs[i] * 2);
        int n = duration_ms * 1000 / (t * 2);
        for (int j = 0; j < n; j++) {
            gpio_put(BUZZER_PIN, 1);
            sleep_us(t);
            gpio_put(BUZZER_PIN, 0);
            sleep_us(t);
        }
    }
}

void play_right() {
    int freqs[2] = {120, 550};
    int duration_ms = 80;
    for (int i = 0; i < 2; i++) {
        int t = 1000000 / (freqs[i] * 2);
        int n = duration_ms * 1000 / (t * 2);
        for (int j = 0; j < n; j++) {
            gpio_put(BUZZER_PIN, 1);
            sleep_us(t);
            gpio_put(BUZZER_PIN, 0);
            sleep_us(t);
        }
    }
}

int muda_sequencia(int sequencia[], int tamanho_sequencia){
    srand((unsigned int)time(NULL));
    sequencia[tamanho_sequencia] = rand() % 4;
    tamanho_sequencia += 1;
    return tamanho_sequencia;
}

void toca_sequencia(int sequencia[], int leds[], int tamanho_sequencia){
    if (mode_red) {
        for(int i = 0; i < tamanho_sequencia; i++) {
            gpio_put(leds[sequencia[i]], 1);
            gpio_put(leds[sequencia[i]], 0);
            sleep_ms(100);
        }
    } else if (mode_blue) {
        for(int i = 0; i < tamanho_sequencia; i++) {
            play_sound(sequencia[i]);
            sleep_ms(100);
        }
    } else { // mode_black
        for(int i = 0; i < tamanho_sequencia; i++) {
            gpio_put(leds[sequencia[i]], 1);
            play_sound(sequencia[i]);
            gpio_put(leds[sequencia[i]], 0);
            sleep_ms(100);
        }
    }
}

void wait_time(){
    clock_t start_time = clock();
    while(!start){}
    clock_t end_time = clock();
    double time_elapsed = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    srand((unsigned int)time_elapsed);
}

int main(){
    stdio_init_all();
    inicializa_dispositivos();
    printf("main\n");

    int tamanho_sequencia = 0;
    int sequencia[100] = {};

    int win_streak = 0;

    int leds[4] = {LED_PIN_B, LED_PIN_G, LED_PIN_R, LED_PIN_Y};

    bool perdeu = false;

    while(true){
        wait_time();
        if ((win_streak == tamanho_sequencia && !perdeu && start) || (perdeu && start)) {
            perdeu = false;

            gpio_put(LED_PIN_B, 1);
            gpio_put(LED_PIN_G, 1);
            gpio_put(LED_PIN_R, 1);
            gpio_put(LED_PIN_Y, 1);

            play_right();

            sleep_ms(100);

            started = true;
            mode_black = false;

            gpio_put(LED_PIN_B, 0);
            gpio_put(LED_PIN_G, 0);
            gpio_put(LED_PIN_R, 0);
            gpio_put(LED_PIN_Y, 0);

            while(!mode_black && !mode_red && !mode_blue){}

            sleep_ms(1000);
            tamanho_sequencia = muda_sequencia(sequencia, tamanho_sequencia);
            toca_sequencia(sequencia, leds, tamanho_sequencia);

            win_streak = 0;

            for(int i = 0; i < tamanho_sequencia; i++){
                
                while (!btn_b_state && !btn_g_state && !btn_r_state && !btn_y_state);
                printf("passou do while\n");

                sleep_ms(100);

                if(btn_b_state && sequencia[i] == 0){
                    gpio_put(LED_PIN_B, 1);
                    sleep_ms(300);
                    gpio_put(LED_PIN_B, 0);
                    btn_b_state = false;
                } else if(btn_g_state && sequencia[i] == 1){
                    gpio_put(LED_PIN_G, 1);
                    sleep_ms(300);
                    gpio_put(LED_PIN_G, 0);
                    btn_g_state = false;
                } else if(btn_r_state && sequencia[i] == 2){
                    gpio_put(LED_PIN_R, 1);
                    sleep_ms(300);
                    gpio_put(LED_PIN_R, 0);
                    btn_r_state = false;
                } else if(btn_y_state && sequencia[i] == 3){
                    gpio_put(LED_PIN_Y, 1);
                    sleep_ms(300);
                    gpio_put(LED_PIN_Y, 0);
                    btn_y_state = false;
                } else {
                    break;
                }
                win_streak += 1;
            }
            if (win_streak == tamanho_sequencia) {
                play_right();
            } else {
                start = false;
                started = false;
                mode_black = false;
                mode_red = false;
                mode_blue = false;

                gpio_put(LED_PIN_B, 1);
                gpio_put(LED_PIN_G, 1);
                gpio_put(LED_PIN_R, 1);
                gpio_put(LED_PIN_Y, 1);

                play_wrong();

                gpio_put(LED_PIN_B, 0);
                gpio_put(LED_PIN_G, 0);
                gpio_put(LED_PIN_R, 0);
                gpio_put(LED_PIN_Y, 0);

                sleep_ms(100);

                for (int i = 0; i < tamanho_sequencia; i++) { // conta a win streak
                    gpio_put(LED_PIN_B, 1);
                    gpio_put(LED_PIN_G, 1);
                    gpio_put(LED_PIN_R, 1);
                    gpio_put(LED_PIN_Y, 1);
                    play_sound(4);
                    gpio_put(LED_PIN_B, 0);
                    gpio_put(LED_PIN_G, 0);
                    gpio_put(LED_PIN_R, 0);
                    gpio_put(LED_PIN_Y, 0);
                    sleep_ms(100);
                }

                win_streak = 0;
    
                memset(sequencia, 0, sizeof(sequencia));
                tamanho_sequencia = 0;

                perdeu = true;

                btn_b_state = false;
                btn_g_state = false;
                btn_r_state = false;
                btn_y_state = false;

                sleep_ms(100);
            }
        }
    }
}
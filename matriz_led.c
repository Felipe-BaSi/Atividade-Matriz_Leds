#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_matrix.pio.h"

// Define o número de LEDs na matriz
#define NUM_PIXELS 25

// Define o pino de saída para os LEDs
#define OUT_PIN 15

// Definições para o teclado matricial
const uint rows[4] = {1, 2, 3, 4};
const uint columns[4] = {5, 6, 7, 8};

// Mapeamento das teclas do teclado matricial
char teclas[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Função para inicializar o teclado matricial
void pico_keypad_init() {
    for (int i = 0; i < 4; i++) {
        gpio_init(rows[i]); // Inicializa os pinos das linhas
        gpio_set_dir(rows[i], GPIO_OUT); // Define os pinos das linhas como saída
        gpio_put(rows[i], 0); // Define o estado inicial das linhas como baixo
    }

    for (int j = 0; j < 4; j++) {
        gpio_init(columns[j]); // Inicializa os pinos das colunas
        gpio_set_dir(columns[j], GPIO_IN); // Define os pinos das colunas como entrada
        gpio_pull_down(columns[j]); // Ativa o pull-down nas colunas
    }
}

// Função para ler a tecla pressionada no teclado matricial
char pico_keypad_get_key() {
    for (int row = 0; row < 4; row++) {
        gpio_put(rows[row], 1); // Define a linha atual como alta

        for (int col = 0; col < 4; col++) {
            if (gpio_get(columns[col])) { // Verifica se a coluna atual está alta
                busy_wait_ms(50); // Aguarda 50 ms para debouncing
                while (gpio_get(columns[col])); // Aguarda até que a tecla seja solta
                gpio_put(rows[row], 0); // Define a linha atual como baixa
                return teclas[row][col]; // Retorna a tecla pressionada
            }
        }

        gpio_put(rows[row], 0); // Define a linha atual como baixa
    }

    return 0; // Retorna 0 se nenhuma tecla foi pressionada
}

// Função para converter valores RGB em um valor de 32 bits
uint32_t matrix_rgb(double b, double r, double g) {
    unsigned char R, G, B;
    R = r * 255; // Converte o valor de vermelho para um valor entre 0 e 255
    G = g * 255; // Converte o valor de verde para um valor entre 0 e 255
    B = b * 255; // Converte o valor de azul para um valor entre 0 e 255
    return (G << 24) | (R << 16) | (B << 8); // Combina os valores RGB em um único valor de 32 bits
}

// Função para enviar os valores RGB para a matriz de LEDs usando PIO
void update_led_matrix(double *r, double *g, double *b, PIO pio, uint sm) {
    uint32_t valor_led;
    for (int16_t i = 0; i < NUM_PIXELS; i++) {
        valor_led = matrix_rgb(b[i], r[i], g[i]); // Converte os valores RGB para um valor de 32 bits
        pio_sm_put_blocking(pio, sm, valor_led); // Envia o valor para a matriz de LEDs
    }
}

// Função para animar os LEDs, alternando as cores
void anim1(int posit, double *r, double *g, double *b) {
    for (int i = 0; i < NUM_PIXELS; i++) {
        if(i < posit){
            r[i] = 0.0;
            g[i] = 0.0;
            b[i] = 0.0;
        } else {
            r[i] = 0.5;
            g[i] = 0.0;
            b[i] = 0.0;
        }
    }

    r[posit] = 0.0;
    g[posit] = 0.7;
    b[posit] = 0.0;

    if(posit > 0 && posit < 24){
        r[posit-1] = 0.0;
        g[posit-1] = 0.7;
        b[posit-1] = 0.0;
    }

    if(posit > 1 && posit < 24){
        r[posit-2] = 0.0; 
        g[posit-2] = 0.7; 
        b[posit-2] = 0.0;
    }
}

// Função principal
int main() {
    PIO pio = pio0; // Seleciona o PIO 0
    bool ok;
    uint16_t i;
    uint32_t valor_led;
    double r[NUM_PIXELS] = {0}, g[NUM_PIXELS] = {0}, b[NUM_PIXELS] = {0}; // Arrays para armazenar os valores RGB dos LEDs
    int posit = 0;
    char caracter_press;

    ok = set_sys_clock_khz(128000, false); // Define a frequência do clock para 128 MHz

    stdio_init_all(); // Inicializa a entrada e saída padrão
    pico_keypad_init(); // Inicializa o teclado matricial

    printf("iniciando a transmissão PIO");
    if (ok) printf("clock set to %ld\n", clock_get_hz(clk_sys)); // Imprime a frequência do clock

    uint offset = pio_add_program(pio, &pio_matrix_program); // Adiciona o programa PIO
    uint sm = pio_claim_unused_sm(pio, true); // Reivindica uma máquina de estado não utilizada
    pio_matrix_program_init(pio, sm, offset, OUT_PIN); // Inicializa o programa PIO

    while (true) {
        caracter_press = pico_keypad_get_key(); // Lê a tecla pressionada

        if(caracter_press){
            printf("\nTecla pressionada: %c\n", caracter_press); // Imprime a tecla pressionada
        }

        switch(caracter_press){
            case '1':
                posit = 0;
                while(posit < NUM_PIXELS){
                    anim1(posit, r, g, b); // Atualiza os valores RGB dos LEDs
                    update_led_matrix(r, g, b, pio, sm); // Envia os valores RGB para a matriz de LEDs
                    posit++;
                    sleep_ms(200); // Velocidade da animação
                }
                posit = 0;
                break;
            

            case 'A':
                for(i = 0; i < NUM_PIXELS; i++){
                    r[i] = 0.0;
                    g[i] = 0.0;
                    b[i] = 0.0;
                }
                update_led_matrix(r, g, b, pio, sm); // Apaga todos os LEDs
                break;
            
            case 'B':
                for(i = 0; i < NUM_PIXELS; i++){
                    r[i] = 0.0;
                    g[i] = 0.0;
                    b[i] = 1.0;
                }
                update_led_matrix(r, g, b, pio, sm); // Acende todos os LEDs em azul
                break;
            
            case 'C':
                for(i = 0; i < NUM_PIXELS; i++){
                    r[i] = 0.8;
                    g[i] = 0.0;
                    b[i] = 0.0;
                }
                update_led_matrix(r, g, b, pio, sm); // Acende todos os LEDs em azul
                break;
            
            case 'D':
                for(i = 0; i < NUM_PIXELS; i++){
                    r[i] = 0.0;
                    g[i] = 0.5;
                    b[i] = 0.0;
                }
                update_led_matrix(r, g, b, pio, sm); // Acende todos os LEDs em azul
                break;
            
            case '#':
                for(i = 0; i < NUM_PIXELS; i++){
                    r[i] = 0.2;
                    g[i] = 0.2;
                    b[i] = 0.2;
                }
                update_led_matrix(r, g, b, pio, sm); // Acende todos os LEDs em azul
                break;
        }
    }
}

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_matrix.pio.h"

// Define o número de LEDs na matriz
#define NUM_PIXELS 25

// Define o pino de saída para os LEDs
#define OUT_PIN 15

// Função para converter valores RGB em um valor de 32 bits
uint32_t matrix_rgb(double b, double r, double g) {
    unsigned char R, G, B;
    R = r * 255; // Converte o valor de vermelho para um valor entre 0 e 255
    G = g * 255; // Converte o valor de verde para um valor entre 0 e 255
    B = b * 255; // Converte o valor de azul para um valor entre 0 e 255
    return (G << 24) | (R << 16) | (B << 8); // Combina os valores RGB em um único valor de 32 bits
}

// Função para enviar os valores RGB para a matriz de LEDs usando PIO
void desenho_pio(double *r, double *g, double *b, PIO pio, uint sm) {
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

    ok = set_sys_clock_khz(128000, false); // Define a frequência do clock para 128 MHz

    stdio_init_all(); // Inicializa a entrada e saída padrão

    printf("iniciando a transmissão PIO");
    if (ok) printf("clock set to %ld\n", clock_get_hz(clk_sys)); // Imprime a frequência do clock

    uint offset = pio_add_program(pio, &pio_matrix_program); // Adiciona o programa PIO
    uint sm = pio_claim_unused_sm(pio, true); // Reivindica uma máquina de estado não utilizada
    pio_matrix_program_init(pio, sm, offset, OUT_PIN); // Inicializa o programa PIO

    while (true) {
        anim1(posit, r, g, b); // Atualiza os valores RGB dos LEDs
        desenho_pio(r, g, b, pio, sm); // Envia os valores RGB para a matriz de LEDs

        if(posit < 25){
            posit++;
        } else {
            posit = 0;
        }

        sleep_ms(200); // Aguarda 200 milissegundos
    }
}
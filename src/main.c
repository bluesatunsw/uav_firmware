// Includes
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"


// # Defines
#define LED_PIN 25


// Function Prototypes
void initialise();
void loop();

/*
 * Main function
*/
int main() {
    initialise();
    
    while (1) {
        loop();
    }
}


/*
 * Initialises the board
*/
void initialise() {
    // Used for debugging?
    bi_decl(bi_program_description("This is a test binary."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    // Initialises the RP2040 standard io library for serial coms
    stdio_init_all();

    // Initialise Pins
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}


/*
 * Main loop
*/
void loop() {
    gpio_put(LED_PIN, 0);
    sleep_ms(250);

    gpio_put(LED_PIN, 1);
    puts("Bluesat UAV Flight Controller Says: 'Hello World'\n");
    sleep_ms(1000);
}
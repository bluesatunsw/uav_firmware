// Includes
#include <FreeRTOS.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"


// # Defines
#define LED_PIN 25


// Function Prototypes
void initialise();


/*
 * Basic Tasks
 */
void led_task() {
    // Initialise Pins
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        gpio_put(LED_PIN, 0);
        vTaskDelay(50);

        gpio_put(LED_PIN, 1);
        puts("Bluesat UAV Flight Controller Says: 'Hello World!'\n");
        vTaskDelay(100);
    }
}

/*
 * Main function
 */
int main() {
    initialise();
    
    // Create Tasks
    xTaskCreate(led_task, "LED Task", 128, NULL, 1, NULL);
    vTaskStartScheduler();

    // Infinite loop - Program will never get to here in execution
    // as FreeRTOS is running from `vTaskStartScheduler()`
    while (true) {};
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
}

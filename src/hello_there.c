#include "hello_there.h"
#include "common/common.h"
#include <FreeRTOS.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

/*
 * #Defines
 */
#define LED_PIN 25

/*
 * Basic LED and Serial Task
 */
void led_task() {
    // Initialise Pins
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        gpio_put(LED_PIN, 0);
        task_delay_ms(800);;

        gpio_put(LED_PIN, 1);
        // puts("Bluesat UAV Flight Controller Says: 'Hello World'\n");
        task_delay_ms(200);
    }
}


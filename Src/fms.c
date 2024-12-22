#include <fms.h>
#include "systick.h"
#include "gpio.h"
#include "uart.h"

DoorState_t current_state = LOCKED;
uint32_t unlock_timer = 0;

void run_state_machine(void) {
    switch (current_state) {
        case LOCKED:
            // No periodic action in locked state
            break;
        case TEMP_UNLOCK:
            if (systick_GetTick() - unlock_timer >= TEMP_UNLOCK_DURATION) {
                gpio_set_door_led_state(0); // Turn off door state LED
                current_state = LOCKED;
            }
            break;
        case PERM_UNLOCK:
            // No periodic action in permanent unlock state
            break;
    }
}

void handle_event(uint8_t event) {
    if (event == 1) { // Single button press
        if(gpio_get_door_led_state()){
            current_state = LOCKED;
            gpio_set_door_led_state(0);
            usart2_send_string("Closing via button!\r\n");
            return;
        }
        gpio_set_door_led_state(1); // Turn on door state LED
        current_state = TEMP_UNLOCK;
        usart2_send_string("Temporal Open!\r\n");
        unlock_timer = systick_GetTick();
    } else if (event == 2) { // Double button press
        gpio_set_door_led_state(1); // Turn on door state LED
        current_state = PERM_UNLOCK;
        usart2_send_string("Permanent Open!\r\n");
    } else if (event == 'O') { // UART OPEN command
        gpio_set_door_led_state(1); // Turn on door state LED
        current_state = TEMP_UNLOCK;
        usart2_send_string("UART OPEN!\r\n");
        unlock_timer = systick_GetTick();
    } else if (event == 'C') { // UART CLOSE command
        gpio_set_door_led_state(0); // Turn off door state LED
        current_state = LOCKED;
        usart2_send_string("UART CLOSE!\r\n");
    }
}


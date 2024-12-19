#include "rcc.h"
#include "gpio.h"

#include "systick.h"
#include <stdint.h>

#define EXTI_BASE 0x40010400
#define EXTI ((EXTI_t *)EXTI_BASE)

#define EXTI15_10_IRQn 40
#define NVIC_ISER1 ((uint32_t *)(0xE000E104)) // NVIC Interrupt Set-Enable Register


#define SYSCFG_BASE 0x40010000
#define SYSCFG ((SYSCFG_t *)SYSCFG_BASE)


#define GPIOA ((GPIO_t *)0x48000000) // Base address of GPIOA
#define GPIOB ((GPIO_t *)0x48000400) // Base address of GPIOA
#define GPIOC ((GPIO_t *)0x48000800) // Base address of GPIOC

#define HEART_BEAT_LED_PIN 5 // Pin 5 of GPIOA
#define DOOR_LED_PIN 4 // Pin 6 of GPIOA

#define DOOR_BUTTON       13  // Pin 13 of GPIOB

void init_gpio_pin(GPIO_t *GPIOx, uint8_t pin, uint8_t mode)
{
    GPIOx->MODER &= ~(0x3 << (pin * 2)); // Clear MODER bits for this pin
    GPIOx->MODER |= (mode << (pin * 2)); // Set MODER bits for this pin
}

void configure_interrupt(GPIO_t *GPIOx, int PIN) {
    int EXTICR_idx = (PIN/4);
    int EXTICR_offset = PIN%4;
    // Configure SYSCFG EXTICR to map EXTIX to PxX
    SYSCFG->EXTICR[EXTICR_idx] &= ~(0xF << 4* EXTICR_offset); // Clear bits for EXTI13
    SYSCFG->EXTICR[EXTICR_idx] |= ((int )(GPIOx-GPIOA)/400 << 4*EXTICR_offset);  // Map EXTI13 to Port B

    // Configure EXTI13 for falling edge trigger
    EXTI->FTSR1 |= (1 << PIN);  // Enable falling trigger
    EXTI->RTSR1 &= ~(1 << PIN); // Disable rising trigger

    // Unmask EXTI13
    EXTI->IMR1 |= (1 << PIN);

    // Configure PA2 and PA3 as no pull-up, no pull-down
    GPIOx->PUPDR |= (1 << (PIN * 2)); // No pull-up, no pull-down for PA2
    init_gpio_pin(GPIOx, PIN, 0x0); // Set BUTTON pin as input

}


void configure_gpio_for_usart(void)
{
    // Enable GPIOA clock
    *RCC_AHB2ENR |= (1 << 0);

    // Configure PA2 (TX) as alternate function
    GPIOA->MODER &= ~(3U << (2 * 2)); // Clear mode bits for PA2
    GPIOA->MODER |= (2U << (2 * 2));  // Set alternate function mode for PA2

    // Configure PA3 (RX) as alternate function
    GPIOA->MODER &= ~(3U << (3 * 2)); // Clear mode bits for PA3
    GPIOA->MODER |= (2U << (3 * 2));  // Set alternate function mode for PA3

    // Set alternate function to AF7 for PA2 and PA3
    GPIOA->AFR[0] &= ~(0xF << (4 * 2)); // Clear AFR bits for PA2
    GPIOA->AFR[0] |= (7U << (4 * 2));   // Set AFR to AF7 for PA2
    GPIOA->AFR[0] &= ~(0xF << (4 * 3)); // Clear AFR bits for PA3
    GPIOA->AFR[0] |= (7U << (4 * 3));   // Set AFR to AF7 for PA3

    // Configure PA2 and PA3 as very high speed
    GPIOA->OSPEEDR |= (3U << (2 * 2)); // Very high speed for PA2
    GPIOA->OSPEEDR |= (3U << (3 * 2)); // Very high speed for PA3

    // Configure PA2 and PA3 as no pull-up, no pull-down
    GPIOA->PUPDR &= ~(3U << (2 * 2)); // No pull-up, no pull-down for PA2
    GPIOA->PUPDR &= ~(3U << (3 * 2)); // No pull-up, no pull-down for PA3
}

void configure_gpio(void)
{

    *RCC_AHB2ENR |= (1 << 0) | (1 << 1); // Enable clock for GPIOA and GPIOC

    // Enable clock for SYSCFG
    *RCC_APB2ENR |= (1 << 0); // RCC_APB2ENR_SYSCFGEN


    init_gpio_pin(GPIOA, HEART_BEAT_LED_PIN, 0x1); // Set LED pin as output
    init_gpio_pin(GPIOA, DOOR_LED_PIN, 0x1); // Set LED pin as output

    configure_interrupt(GPIOB, DOOR_BUTTON);

    // Enable EXTI15_10 interrupt
    *NVIC_ISER1 |= (1 << (EXTI15_10_IRQn - 32));

    configure_gpio_for_usart();
}

// Emula el comprtamiento de la puerta
void gpio_set_door_led_state(uint8_t state) {
    if (state) {
        GPIOA->ODR |= (1 << DOOR_LED_PIN); // encender LED estado puerta
    } else {
        GPIOA->ODR &= ~(1 << DOOR_LED_PIN); // apagar LED estado puerta
    }
}

uint8_t gpio_get_door_led_state(void) {
    return GPIOA->ODR & (1 << DOOR_LED_PIN); // encender LED estado puerta
}

void gpio_toggle_heartbeat_led(void) {
    GPIOA->ODR ^= (1 << 5);
}

volatile uint8_t button_pressed = 0; // Flag to indicate button press
uint8_t button_driver_get_event(void)
{
    uint8_t _temp=button_pressed;
    button_pressed=0;
    return _temp;
}

uint32_t b1_tick = 0;
void detect_button_press(void)
{
    if (systick_GetTick() - b1_tick < 50) {
        return; // Ignore bounces of less than 50 ms
    } else if (systick_GetTick() - b1_tick > 5000) {
        button_pressed = 1; // single press
    } else {
        button_pressed = 2; // double press
    }

    b1_tick = systick_GetTick();
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI->PR1 & (1 << DOOR_BUTTON)) {
        EXTI->PR1 = (1 << DOOR_BUTTON); // Clear pending bit
        detect_button_press();
    }
}
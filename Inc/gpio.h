#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

typedef struct {
    volatile uint32_t MEMRMP;
    volatile uint32_t CFGR1;
    volatile uint32_t EXTICR[4];
    volatile uint32_t CFGR2;
} SYSCFG_t;

typedef struct {
    volatile uint32_t IMR1;
    volatile uint32_t EMR1;
    volatile uint32_t RTSR1;
    volatile uint32_t FTSR1;
    volatile uint32_t SWIER1;
    volatile uint32_t PR1;
    volatile uint32_t IMR2;
    volatile uint32_t EMR2;
    volatile uint32_t RTSR2;
    volatile uint32_t FTSR2;
    volatile uint32_t SWIER2;
    volatile uint32_t PR2;
} EXTI_t;


typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
    volatile uint32_t BRR;

} GPIO_t;

/**
 * @brief Función para configurar los GPIOS (engloba toda la complejidad e interacciones)
 *
 * @param[void] void Esta función no recibe ninguna entrada.
 */
void configure_gpio(void);


/**
 * @brief Setea el estado del led que simula la puerta:
 * state = 0: LED apagado (puerta bloqueada)
 * state = 1: LED encendido (puerta desbloqueada)
 * @param[uint8_t] state Estado del led de acuerdo a la descripción anterior.
 */
void gpio_set_door_led_state(uint8_t state);

/**
 * @brief Cambia el estado del led de hearthbeat negándolo.
 *
 * @param[void] void Esta función no recibe ninguna entrada.
 */
void gpio_toggle_heartbeat_led(void);

/**
 * @brief Obtiene el evento de pulsación más reciente de acuerdo a las siguientes reglas:
 * - 1: se ha realizado una pulsación.
 * - 2: Se han realizado dos pulsaciones en menos de 5 segundos (estado temporal)
 * - 0: No hay interacción reciente.
 *
 *   depués de retornar el estado se resetea a 0 automáticamente.
 *
 * @param[void] void Esta función no recibe ninguna entrada.
 * @return El evento más reciente
 */
uint8_t button_driver_get_event(void);


/**
 * @brief Configura PA2 y PA3 para operar en transmisión de datos de UART.
 *
 * @param[void] void Esta función no recibe ninguna entrada.
 */
void configure_gpio_for_usart(void);

/**
 * @brief Obtiene el estado actual del LED.
 *
 * @return Retorna si el led está encendido
 */
uint8_t gpio_get_door_led_state(void);

#endif // GPIO_H


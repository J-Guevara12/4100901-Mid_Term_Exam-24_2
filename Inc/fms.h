#include <stdint.h>

#define TEMP_UNLOCK_DURATION 5000 // Duration in ms for temporary unlock

typedef enum {
    LOCKED,
    TEMP_UNLOCK,
    PERM_UNLOCK
} DoorState_t;

/**
 * @brief Actualiza la máquina de estdo basado en las variables estáticas de presionado de botón, 
 * comando recibido y el tiempo ocurrido desde el último vambio.
 *
 * @param[void] Void Esta función no recibe nada.
 */
void run_state_machine(void);

/**
 * @brief Maneja un evento en formato uint8_t que puede ser un caracter recibido por UART o la cantidad de pulsaciones hechas al botón.
 *
 * @param[uint8_t] event El evento que se va a procesar y que actualiza el estado de la máquina de estado.
 */
void handle_event(uint8_t event);


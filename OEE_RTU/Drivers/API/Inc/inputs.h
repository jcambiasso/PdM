/**
 * @file inputs.h
 * @author Javier Cambiasso
 * @brief Generic input handler.
 * @version 0.1
 * @date 2023-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef inputs_h
#define inputs_h

#include <stddef.h>
#include <stdbool.h>
#include <API_timer.h>

#ifndef MAX_PCINPUTS
#define MAX_PCINPUTS 8
#endif


#ifdef __cplusplus
extern "C"{
#endif

typedef enum{
	EXTERNAL,
	PINCHANGE,
	NONE
}InputTypes_t;

typedef enum{
	CHANGE,
	FALLING,
	RISING,
	HIGH,
	LOW,
}InputModes_t;

typedef void(*InputCallback)(void);

typedef struct input_s{
    bool negate;
    volatile bool state;
    volatile bool lastState;
    volatile bool debounce;
    volatile uint8_t stage;
    uint8_t mode;
    bool engaged;
    uint8_t pin;
    Timer_t debounceTimer;
    InputCallback callback;
    uint8_t type;
    bool Lock;
} Input_t;

///Arq specific functions. These need to be implemented in some source.

/**
 * @brief Reads pin input
 * 
 * @param pin Board pin number. 
 * @return true Return true if high
 * @return false Return false if low
 */
bool readPin(uint8_t pin);

/**
 * @brief Enables Pin change interrupt feature on specified pin.
 * 
 * @param pin Board pin number.
 */
void enablePinInterrupt(uint8_t pin);

/**
 * @brief Enables global interrupt (External and Pinchange)
 * 
 */
void enableGlobalInputInterrupts();

/**
 * @brief Disables global interrupt (External and Pinchange)
 * 
 */
void disableGlobalInputInterrupts();

/**
 * @brief Register an External input to the corresponding interrupt vector.
 * 
 * @param pin Board pint number
 * @param mode FALLING, CHANGE, LEVEL, RISING
 */
void attachExternalInterrupt(uint8_t pin, void(*)(), InputModes_t mode);

/**
 * @brief Inits the inputs (pin direction, pullups, sets types, etc.)
 * 
 * @param input Input pointer/
 */
void primeInput(Input_t* input);
///


/**
 * @brief initializes the input object.
 * 
 * @param input Pointer to input object
 * @param pin Board pin number
 * @param interval Debounce timer interval.
 * @param mode FALLING, RISING, CHANGE, LEVEL.
 * @param cb Function that gets called when input is valid.
 */
void input_init(Input_t* input, uint8_t pin, uint32_t interval, InputModes_t mode, InputCallback cb, InputTypes_t interruptType);

/**
 * @brief Evaluates the input according to configuration. 
 * 
 * @param input Pointer to input object
 * @param autoUpdate Noise may change the input before validation passes. In those situations, set this to false and manually update.
 * @return true Returns true is the input meets all requeriments to trigger the callback.
 * @return false Returns false if the input does not meets all requeriments to trigger the callback.
 */
uint8_t input_eval(Input_t* input);

/**
 * @brief Updates the phisical state of the pin. Negation applies
 * 
 * @param input Pointer to input object
 * @return true 
 * @return false 
 */
bool input_update(Input_t* input);

/**
 * @brief Registers the interrupt to the callback queue according to type.
 * @note Depending on target, not all pins are compatible with all interrupt types.
 * @param input Pointer to input object
 */
void input_addInterrupt(Input_t* input);

/**
 * @brief Attaches or reataches callback routine to input
 * @param input Pointer to input object
 * @retval None
 */
void input_attachCallback(Input_t* input, InputCallback callback);

extern void(*inputs_irqHandler)();
/**
 * @brief Interrupt routine to call in ISR
 * 
 */
void input_irq(void) __attribute__((noinline));

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class Input : protected Input_t {
    public:
        Input(uint8_t pin); //Minimal constructor.
        /**
         * @brief Input class that deals with debounce and input modes for you.
         * 
         * @param pin Pin where the actual input is located.
         * @param interval Debounce timer in ms
         * @param mode 
         * @param cb Callback function.
         */
        Input(uint8_t pin,uint32_t interval,int mode, InputCallback cb);
        void attachCallback(InputCallback cb);
        /**
         * @brief Invokes the Input's callback if configuration conditions are met (Debounce, mode)
         * 
         * @returns If the input meets the conditions to be set.
         */
        uint8_t  evaluate(bool autoUpdate = true);
        /**
         * @brief Check if this input has been set. Has no impact on the rest of the functionality.
         * 
         * @param clear You may clear the entry or leave it to check is the input was ever fired.
         */
        bool isSet(bool clear);
        bool updateState();

    protected:
        void init(uint8_t pin, uint32_t interval, int mode, InputCallback cb);
};

class ExternalInterruptInput : public Input{
    public:
        ExternalInterruptInput(uint8_t pin);
        /**
         * @brief Special input that can be attached to a hardware interrupt routine.
         * 
         * @param interval Debounce timer.
         * @param pin  PIN number.
         * @param mode FALLING, RISING, ETC.
         * @param cb Callback function, taking no arguments and returning none.
         */
        ExternalInterruptInput(uint8_t pin,uint32_t interval,int mode, InputCallback cb);
        static void allowInterrupts();
        static void disableInterrupts();
        virtual void addInterrupt();
};

class PinChangeInterruptInput : public ExternalInterruptInput{
    public:
        PinChangeInterruptInput(uint8_t pin);
        PinChangeInterruptInput(uint8_t pin,uint32_t interval,int mode, InputCallback cb);
        static void allowInterrupts();
        static void disableInterrupts();
        void addInterrupt();
    private:
        static uint8_t index;
        static bool initialized;
        void emptyVector();     
};
#endif //if __cplusplus
#endif //INPUTS_H

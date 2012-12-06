/* gpio.h */

/* GPIO memory-mapped I/O address */
#define GPIO_BASE 0xB8000060

/* GPIO bits for pin direction */
#define GPIO_DIR_IN        0x00 /**< set pin for input                  */
#define GPIO_DIR_OUT       0x01 /**< set pin for output                 */

/* GPIO bit flags for pins */
#define GPIO0              0x01
#define GPIO1              0x02
#define GPIO2              0x04
#define GPIO3              0x08
#define GPIO4              0x10
#define GPIO5              0x20
#define GPIO6              0x40
#define GPIO7              0x80

#define GPIO_PIN_COUNT     8

/* Buttons */
#define GPIO_BUT_CISCO    GPIO4 /**< Front Cisco button                 */
#define GPIO_BUT_RESET    GPIO6 /**< Back reset button                  */

/* LEDs */
#define GPIO_LED_WLAN     GPIO0 /**< WLAN LED                           */
#define GPIO_LED_POWER    GPIO1 /**< Power LED (hardware controlled)    */
#define GPIO_LED_CISCOWHT GPIO2 /**< White Cisco LED                    */
#define GPIO_LED_CISCOONG GPIO3 /**< Orange Cisco LED                   */
#define GPIO_LED_DMZ      GPIO7 /**< DMZ LED                            */

/**
 * Control and status registers for the GPIO.
 */
struct gpio_csreg
{
    volatile uint32 gp_input;       /**< input                              */
    volatile uint32 gp_output;      /**< output                             */
    volatile uint32 gp_enable;      /**< direction                          */
    volatile uint32 gp_control;     /**< usage unkmown                      */
};

/* LED control functions */

/**
 * Turns an LED on
 * @param n GPIO bit for LED (use GPIO_LED_* constants)
 */
#define gpioLEDOn(n) ((struct gpio_csreg *)GPIO_BASE)->gp_enable |= (n); \
		((struct gpio_csreg *)GPIO_BASE)->gp_output &= ~(n)

/**
 * Turns an LED off
 * @param n GPIO bit for LED (use GPIO_LED_* constants)
 */
#define gpioLEDOff(n) ((struct gpio_csreg *)GPIO_BASE)->gp_enable &= ~(n)

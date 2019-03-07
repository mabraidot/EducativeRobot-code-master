
// PIN
#define SLAVE_ACTIVATE_PIN          4
#define FUNCTION_ACTIVATE_PIN       2
#define BUZZER                      3
#define RUN_BUTTON                  7
#define RUN_LED                     8
#define STEPS_BUTTON                5
#define STEPS_LED                   6
#define RF_CE                       9
#define RF_CSN                      10

// I2C
#define SLAVE_ADDRESS               0x08
#define SLAVE_MODIFIER_ADDRESS      0x09
#define SLAVE_START_ADDRESS         20

// RF
#define RF_CLIENT_ADDRESS           1
#define RF_SERVER_ADDRESS           2


#define SLAVES_COUNT                25
#define SLAVES_MODIFIERS_COUNT      3
#define FUNCTION_COUNT              10
#define FUNCTION_MODIFIERS_COUNT    3

// SLAVE MODES
#define MODE_MODIFIER_LOOP          1
#define MODE_SLAVE_FORWARD_ARROW    2
#define MODE_SLAVE_LEFT_ARROW       2
#define MODE_SLAVE_RIGHT_ARROW      3
#define MODE_FUNCTION               4

// SLAVE STATES
#define STATE_ADDRESS               0
#define STATE_GATE                  1

#define STATE_LED                   2
#define STATE_LED_OFF               0
#define STATE_LED_ON                1
#define STATE_LED_BLINK             2

#define STATE_ACTIVATED             3
#define STATE_FUNCTION              4
#define STATE_VALUE                 5


// GENERAL
#define DEBUG                       true
#define SILENT                      false
// Button Debouncing (Kenneth A. Kuhn algorithm)
#define DEBOUNCE_TIME               0.3
#define DEBOUNCE_SAMPLE_FREQUENCY   10
#define DEBOUNCE_MAXIMUM            (DEBOUNCE_TIME * DEBOUNCE_SAMPLE_FREQUENCY)
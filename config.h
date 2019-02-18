
// PIN
#define SLAVE_ACTIVATE_PIN          4
#define FUNCTION_ACTIVATE_PIN       2
#define BUZZER                      3
#define RUN_BUTTON                  7
#define RUN_LED                     8
#define STEPS_BUTTON                5
#define STEPS_LED                   6

// I2C
#define SLAVE_ADDRESS               0x01
#define SLAVE_MODIFIER_ADDRESS      0x02
#define FUNCTION_ADDRESS            0x03
#define SLAVE_START_ADDRESS         10

#define SLAVES_COUNT                50
#define SLAVES_MODIFIERS_COUNT      5
#define FUNCTION_COUNT              10
#define FUNCTION_MODIFIERS_COUNT    5

// SLAVE FUNCTIONS
#define MODIFIER_LOOP               1
#define SLAVE_FORWARD_ARROW         2
#define SLAVE_LEFT_ARROW            2
#define SLAVE_RIGHT_ARROW           3

// SLAVE STATES
#define SLAVE_STATE_ADDRESS         0
#define SLAVE_STATE_GATE            1
#define SLAVE_STATE_LED             2
#define SLAVE_STATE_ACTIVATED       3
#define SLAVE_STATE_FUNCTION        4
#define SLAVE_STATE_VALUE           5

// GENERAL
#define DEBUG                       true
#define SILENT                      false

#include <stdbool.h>
#include <stdio.h>

#include "driver/gpio.h"

#define LED_SLEEP_AFTER_COUNT 30

#define GPIO_OUTPUT_LED_RED 28     // Select GPIO28
#define GPIO_OUTPUT_LED_YELLOW 27  // Select GPIO27
#define GPIO_OUTPUT_LED_GREEN 23   // Select GPIO23

#define GPIO_OUTPUT_PIN_SEL                                            \
    ((1ULL << GPIO_OUTPUT_LED_RED) | (1ULL << GPIO_OUTPUT_LED_GREEN) | \
     (1ULL << GPIO_OUTPUT_LED_YELLOW))

typedef struct {
    bool state;
} led_item_t;

void led_turn_on(led_item_t* s) {
    if (s) {
        s->state = true;
    }
}

void led_turn_off(led_item_t* s) {
    if (s) {
        s->state = false;
    }
}

typedef enum {
    AQ_HAZARDOUS,                       // 0
    AQ_VERY_UNHEALTHY,                  // 1
    AQ_MORE_THAN_UNHEALTHY,             // 2
    AQ_UNHEALTHY,                       // 3
    AQ_UNHEALTHY_FOR_SENSITIVE_GROUPS,  // 4
    AQ_MODERATE,                        // 5
    AQ_GOOD,                            // 6
    AQ_UNKNOWN
} air_quality_t;

const char* air_quality_to_string(air_quality_t aq) {
    switch (aq) {
        case AQ_HAZARDOUS:
            return "Hazardous";
        case AQ_VERY_UNHEALTHY:
            return "Very Unhealthy";
        case AQ_MORE_THAN_UNHEALTHY:
            return "More than Unhealthy";
        case AQ_UNHEALTHY:
            return "Unhealthy";
        case AQ_UNHEALTHY_FOR_SENSITIVE_GROUPS:
            return "Unhealthy for Sensitive Groups";
        case AQ_MODERATE:
            return "Moderate";
        case AQ_GOOD:
            return "Good";
        default:
            return "Unknown";
    }
}

typedef struct {
    led_item_t ledRed;
    led_item_t ledYellow;
    led_item_t ledGreen;

    air_quality_t curr_quality;
    air_quality_t prev_quality;

    bool is_dirty;
    int16_t sticky_count;
} leds_state_t;

static leds_state_t g_leds = {
    .curr_quality = AQ_UNKNOWN,
    .prev_quality = AQ_UNKNOWN,
    .ledRed.state = false,
    .ledYellow.state = false,
    .ledGreen.state = false,
    .is_dirty = false,
    .sticky_count = 0,
};

void LedsSetup() {
    gpio_config_t io_conf = {
        .pin_bit_mask = GPIO_OUTPUT_PIN_SEL,
        .mode = GPIO_MODE_OUTPUT,           // Set as output mode
        .pull_up_en = GPIO_PULLUP_DISABLE,  // Disable internal pull-up resistor
        .pull_down_en =
            GPIO_PULLDOWN_DISABLE,      // Disable internal pull-down resistor
        .intr_type = GPIO_INTR_DISABLE  // Disable interrupts
    };
    gpio_config(&io_conf);
}

void AllLedsOn() {
    led_turn_on(&g_leds.ledRed);
    led_turn_on(&g_leds.ledYellow);
    led_turn_on(&g_leds.ledGreen);
}

void AllLedsOff() {
    led_turn_off(&g_leds.ledRed);
    led_turn_off(&g_leds.ledYellow);
    led_turn_off(&g_leds.ledGreen);
}

void RedLedTurnOn() { led_turn_on(&g_leds.ledRed); }

void RedLedTurnOff() { led_turn_off(&g_leds.ledRed); }

void YellowLedTurnOn() { led_turn_on(&g_leds.ledYellow); }

void YellowLedTurnOff() { led_turn_off(&g_leds.ledYellow); }

void GreenLedTurnOn() { led_turn_on(&g_leds.ledGreen); }

void GreenLedTurnOff() { led_turn_off(&g_leds.ledGreen); }

void NextStateLeds(float iaq) {
    g_leds.prev_quality = g_leds.curr_quality;
    if (iaq > 301) {
        g_leds.curr_quality = AQ_HAZARDOUS;
        RedLedTurnOn();
        YellowLedTurnOff();
        GreenLedTurnOff();
    } else if (iaq > 250 && iaq <= 300) {
        g_leds.curr_quality = AQ_VERY_UNHEALTHY;
        RedLedTurnOn();
        YellowLedTurnOff();
        GreenLedTurnOff();
    } else if (iaq > 200 && iaq <= 250) {
        g_leds.curr_quality = AQ_MORE_THAN_UNHEALTHY;
        RedLedTurnOn();
        YellowLedTurnOn();
        GreenLedTurnOff();
    } else if (iaq > 150 && iaq <= 200) {
        g_leds.curr_quality = AQ_UNHEALTHY;
        RedLedTurnOn();
        YellowLedTurnOn();
        GreenLedTurnOff();
    } else if (iaq > 100 && iaq <= 150) {
        g_leds.curr_quality = AQ_UNHEALTHY_FOR_SENSITIVE_GROUPS;
        RedLedTurnOff();
        YellowLedTurnOn();
        GreenLedTurnOff();
    } else if (iaq > 50 && iaq <= 100) {
        g_leds.curr_quality = AQ_MODERATE;
        RedLedTurnOff();
        YellowLedTurnOn();
        GreenLedTurnOn();
    } else if (iaq >= 00 && iaq <= 50) {
        g_leds.curr_quality = AQ_GOOD;
        RedLedTurnOff();
        YellowLedTurnOff();
        GreenLedTurnOn();
    }
    bool has_changed = g_leds.prev_quality != g_leds.curr_quality;
    if (has_changed) {
        printf("Current air quality: %s (code %d)\n",
               air_quality_to_string(g_leds.curr_quality), g_leds.curr_quality);
        g_leds.is_dirty = true;
        g_leds.sticky_count = 0;
    } else {
        if (g_leds.sticky_count < LED_SLEEP_AFTER_COUNT) {
            g_leds.sticky_count++;
        } else {
            if (g_leds.ledGreen.state || g_leds.ledRed.state ||
                g_leds.ledYellow.state) {
                AllLedsOff();
                g_leds.is_dirty = true;
            }
        }
    }
}

void UpdateLeds() {
    if (!g_leds.is_dirty) {
        return;
    }
    gpio_set_level(GPIO_OUTPUT_LED_RED, g_leds.ledRed.state ? 1 : 0);
    gpio_set_level(GPIO_OUTPUT_LED_YELLOW, g_leds.ledYellow.state ? 1 : 0);
    gpio_set_level(GPIO_OUTPUT_LED_GREEN, g_leds.ledGreen.state ? 1 : 0);

    g_leds.is_dirty = false;
}
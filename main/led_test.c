/* used to test led on/off */
#include "driver/gpio.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#define GPIO_OUTPUT_LED_RED 28     // Select GPIO28
#define GPIO_OUTPUT_LED_YELLOW 27  // Select GPIO27
#define GPIO_OUTPUT_LED_GREEN 23    // Select GPIO23

#define GPIO_OUTPUT_PIN_SEL                                            \
    ((1ULL << GPIO_OUTPUT_LED_RED) | (1ULL << GPIO_OUTPUT_LED_GREEN) | \
     (1ULL << GPIO_OUTPUT_LED_YELLOW))

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    gpio_config_t io_conf = {
        .pin_bit_mask = GPIO_OUTPUT_PIN_SEL,
        .mode = GPIO_MODE_OUTPUT,           // Set as output mode
        .pull_up_en = GPIO_PULLUP_DISABLE,  // Disable internal pull-up resistor
        .pull_down_en =
            GPIO_PULLDOWN_DISABLE,      // Disable internal pull-down resistor
        .intr_type = GPIO_INTR_DISABLE  // Disable interrupts
    };
    gpio_config(&io_conf);

    // TODO: use lib/led_state that does not compile
    int cnt = 0;
    while (1) {
        int val = cnt % 2;
        printf("cnt: %d -> val = %d\n", cnt++, val);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_LED_RED, val);
        gpio_set_level(GPIO_OUTPUT_LED_YELLOW, val);
        gpio_set_level(GPIO_OUTPUT_LED_GREEN, val);
    }
}

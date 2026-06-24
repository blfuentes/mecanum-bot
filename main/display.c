#include "display.h"

#include <esp_log.h>
#include <stdio.h>
#include <string.h>

static const char* SCREEN_TAG   = "DISPLAY";
static const int DISPLAY_WIDTH  = 128;
static const int DISPLAY_HEIGHT = 64;
static const int PAGE_1         = 0;
static const int PAGE_2         = 1;
static const int PAGE_3         = 2;
static const int PAGE_4         = 3;
static const int PAGE_5         = 4;
static const int PAGE_6         = 5;
static const int PAGE_7         = 6;
static const int PAGE_8         = 7;

void display_init(Display* display) {
    // OLED
    ESP_LOGI(SCREEN_TAG, "Initializing SSD1306 OLED display...");
    ESP_LOGI(SCREEN_TAG, "INTERFACE is i2c");
    ESP_LOGI(SCREEN_TAG, "CONFIG_SDA_GPIO=%d", CONFIG_SDA_GPIO);
    ESP_LOGI(SCREEN_TAG, "CONFIG_SCL_GPIO=%d", CONFIG_SCL_GPIO);
    ESP_LOGI(SCREEN_TAG, "CONFIG_RESET_GPIO=%d", CONFIG_RESET_GPIO);
    ESP_LOGI(SCREEN_TAG, "Panel is %dx%d", DISPLAY_WIDTH, DISPLAY_HEIGHT);

    i2c_master_init(&display->dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);

    memset(display->lines, 0, sizeof(display->lines));
    display->consecutive_errors = 0;

    ssd1306_init(&display->dev, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    ssd1306_clear_screen(&display->dev, false);
    ssd1306_contrast(&display->dev, 0xff);

    // strncpy(display->lines[DISPLAY_LINE_0], TITLE, DISPLAY_BUFFER_SIZE - 1);
    // display->lines[DISPLAY_LINE_0][DISPLAY_BUFFER_SIZE - 1] = '\0';
    // ssd1306_display_text(&display->dev, PAGE_1, display->lines[DISPLAY_LINE_0], 18, false);

    ESP_LOGI(SCREEN_TAG, "SSD1306 initialized");
}

static void display_reinit(Display* display) {
    ESP_LOGW(SCREEN_TAG, "Re-initializing display after I2C errors");
    ssd1306_init(&display->dev, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    ssd1306_clear_screen(&display->dev, false);
    ssd1306_contrast(&display->dev, 0xff);
    ssd1306_display_text(&display->dev, PAGE_1, display->lines[DISPLAY_LINE_0], 18, false);
}

void display_show_status(Display* display, const char* line0, const char* line1, const char* line2,
                         const char* line3, const char* line4, const char* line5, const char* line6,
                         const char* line7) {
    if (display == NULL) {
        return;
    }

    const char* zero_line    = (line0 != NULL) ? line0 : "";
    const char* first_line   = (line1 != NULL) ? line1 : "";
    const char* second_line  = (line2 != NULL) ? line2 : "";
    const char* third_line   = (line3 != NULL) ? line3 : "";
    const char* fourth_line  = (line4 != NULL) ? line4 : "";
    const char* fifth_line   = (line5 != NULL) ? line5 : "";
    const char* sixth_line   = (line6 != NULL) ? line6 : "";
    const char* seventh_line = (line7 != NULL) ? line7 : "";

    memset(display->lines[DISPLAY_LINE_0], ' ', DISPLAY_BUFFER_SIZE);
    display->lines[DISPLAY_LINE_0][DISPLAY_BUFFER_SIZE - 1] = '\0';
    snprintf(display->lines[DISPLAY_LINE_0], DISPLAY_BUFFER_SIZE, "%.16s", zero_line);

    // Line 1
    memset(display->lines[DISPLAY_LINE_1], ' ', DISPLAY_BUFFER_SIZE);
    display->lines[DISPLAY_LINE_1][DISPLAY_BUFFER_SIZE - 1] = '\0';
    snprintf(display->lines[DISPLAY_LINE_1], DISPLAY_BUFFER_SIZE, "%.16s", first_line);

    // Line 2
    memset(display->lines[DISPLAY_LINE_2], ' ', DISPLAY_BUFFER_SIZE);
    display->lines[DISPLAY_LINE_2][DISPLAY_BUFFER_SIZE - 1] = '\0';
    snprintf(display->lines[DISPLAY_LINE_2], DISPLAY_BUFFER_SIZE, "%.16s", second_line);

    // Line 3
    memset(display->lines[DISPLAY_LINE_3], ' ', DISPLAY_BUFFER_SIZE);
    display->lines[DISPLAY_LINE_3][DISPLAY_BUFFER_SIZE - 1] = '\0';
    snprintf(display->lines[DISPLAY_LINE_3], DISPLAY_BUFFER_SIZE, "%.16s", third_line);

    // Line 4
    memset(display->lines[DISPLAY_LINE_4], ' ', DISPLAY_BUFFER_SIZE);
    display->lines[DISPLAY_LINE_4][DISPLAY_BUFFER_SIZE - 1] = '\0';
    snprintf(display->lines[DISPLAY_LINE_4], DISPLAY_BUFFER_SIZE, "%.16s", fourth_line);

    // Line 5
    memset(display->lines[DISPLAY_LINE_5], ' ', DISPLAY_BUFFER_SIZE);
    display->lines[DISPLAY_LINE_5][DISPLAY_BUFFER_SIZE - 1] = '\0';
    snprintf(display->lines[DISPLAY_LINE_5], DISPLAY_BUFFER_SIZE, "%.16s", fifth_line);

    // Line 6
    memset(display->lines[DISPLAY_LINE_6], ' ', DISPLAY_BUFFER_SIZE);
    display->lines[DISPLAY_LINE_6][DISPLAY_BUFFER_SIZE - 1] = '\0';
    snprintf(display->lines[DISPLAY_LINE_6], DISPLAY_BUFFER_SIZE, "%.16s", sixth_line);

    // Line 7
    memset(display->lines[DISPLAY_LINE_7], ' ', DISPLAY_BUFFER_SIZE);
    display->lines[DISPLAY_LINE_7][DISPLAY_BUFFER_SIZE - 1] = '\0';
    snprintf(display->lines[DISPLAY_LINE_7], DISPLAY_BUFFER_SIZE, "%.16s", seventh_line);

    display->debug_mode = false;
    display_update(display);
}

void display_matrix_content(Display* display, const char* content[]) {
    display_show_status(display, content[0], content[1], content[2], content[3], content[4],
                        content[5], content[6], content[7]);
}

bool display_update(Display* display) {

    // Reset error count
    i2c_display_get_and_clear_error_count();

    ssd1306_display_text(&display->dev, PAGE_1, display->lines[DISPLAY_LINE_0], 18, false);
    ssd1306_display_text(&display->dev, PAGE_2, display->lines[DISPLAY_LINE_1], 18, false);
    ssd1306_display_text(&display->dev, PAGE_3, display->lines[DISPLAY_LINE_2], 18, false);
    ssd1306_display_text(&display->dev, PAGE_4, display->lines[DISPLAY_LINE_3], 18, false);
    ssd1306_display_text(&display->dev, PAGE_5, display->lines[DISPLAY_LINE_4], 18, false);
    ssd1306_display_text(&display->dev, PAGE_6, display->lines[DISPLAY_LINE_5], 18, false);
    ssd1306_display_text(&display->dev, PAGE_7, display->lines[DISPLAY_LINE_6], 18, false);
    ssd1306_display_text(&display->dev, PAGE_8, display->lines[DISPLAY_LINE_7], 18, false);

    // if (display->debug_mode) {
    //     ssd1306_display_text(&display->dev, PAGE_2, display->lines[DISPLAY_LINE_1], 18, false);
    // } else {
    //     // Clear debug page if not in debug mode to avoid showing stale data.
    //     ssd1306_display_text(&display->dev, PAGE_2, " ", 1, false);
    // }

    int errors = i2c_display_get_and_clear_error_count();
    if (errors == 0) {
        display->consecutive_errors = 0;
        return true;
    }

    display->consecutive_errors++;
    ESP_LOGW(SCREEN_TAG, "I2C errors during update (%d), consecutive=%d", errors,
             (int)display->consecutive_errors);

    if (display->consecutive_errors >= DISPLAY_MAX_CONSECUTIVE_ERRORS) {
        // Attempt bus recovery first.
        esp_err_t err = i2c_display_recover_bus();
        if (err != ESP_OK) {
            ESP_LOGE(SCREEN_TAG, "I2C bus recovery failed: %s", esp_err_to_name(err));
        }
        // Re-initialize the display controller.
        display_reinit(display);
        display->consecutive_errors = 0;
    }

    return false;
}
#ifndef DISPLAY_H__
#define DISPLAY_H__

#include <ssd1306.h>
#include <stdbool.h>
#include <stdint.h>

/** Maximum printable characters per display line including null terminator. */
#define DISPLAY_BUFFER_SIZE 17
/** Number of cached text rows managed by the display module. */
#define DISPLAY_NUM_LINES 8
/** Threshold of consecutive I2C failures before display recovery is attempted. */
#define DISPLAY_MAX_CONSECUTIVE_ERRORS 3

static const int DISPLAY_LINE_0 = 0;
static const int DISPLAY_LINE_1 = 1;
static const int DISPLAY_LINE_2 = 2;
static const int DISPLAY_LINE_3 = 3;
static const int DISPLAY_LINE_4 = 4;
static const int DISPLAY_LINE_5 = 5;
static const int DISPLAY_LINE_6 = 6;
static const int DISPLAY_LINE_7 = 7;

/**
 * @brief Display runtime state used by the SSD1306 UI module.
 */
typedef struct {
    /** SSD1306 low-level driver descriptor. */
    SSD1306_t dev;
    /** Cached text lines pushed to the OLED pages. */
    char lines[DISPLAY_NUM_LINES][DISPLAY_BUFFER_SIZE];
    /** Count of consecutive update errors detected by I2C backend. */
    uint8_t consecutive_errors;
    /** Enables rendering of debug information on the debug page. */
    bool debug_mode;
} Display;

/**
 * @brief Initialize SSD1306 and prepare the display state.
 * @param display Pointer to display state struct.
 */
void display_init(Display* display);

/**
 * @brief Render cached lines to OLED and run link-recovery when needed.
 * @param display Pointer to initialized display state.
 * @return true if the update completed without I2C errors, false otherwise.
 */
bool display_update(Display* display);

/**
 * @brief Display short boot-time status text on the OLED.
 * @param display Pointer to initialized display state.
 * @param line1 First status line to render.
 * @param line2 Second status line to render.
 * @param line3 Third status line to render.
 * @param line4 Fourth status line to render.
 * @param line5 Fifth status line to render.
 * @param line6 Sixth status line to render.
 * @param line7 Seventh status line to render.
 */
void display_show_status(Display* display, const char* line1, const char* line2, const char* line3,
                         const char* line4, const char* line5, const char* line6,
                         const char* line7);

#endif  // DISPLAY_H__
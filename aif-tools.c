#include "aif.h"
#include <stdint.h>
#include <stdio.h>

// Takes in a RGB color and brightens it by the given percentage amount
uint32_t brighten_rgb(uint32_t color, int amount);

void stage1_info(int n_files, const char **files) {
    // Remember to look at the constants and helper functions we have provided you
    // in aif.h and good luck!

    printf("<%s>:\n", "duck.aif");
    printf("File-size: %ld bytes\n", 67l);
    printf("Checksum: %02x %02x\n", 0xbe, 0xef);
    printf("Pixel format: %s\n", "take a look at the helper functions in aif.h");
    printf("Compression: %s\n", "same here");
    printf("Width: %d px\n", 1337);
    printf("Height: %d px\n", 42);
}

void stage2_brighten(int amount, const char *in_file, const char *out_file) {

}

void stage3_convert_color(const char *color, const char *in_file, const char *out_file) {

}

void stage4_decompress(const char *in_file, const char *out_file) {

}

void stage5_compress(const char *in_file, const char *out_file) {

}

///////////////////////////////
// PROVIDED CODE
// It is best you do not modify anything below this line
///////////////////////////////
uint32_t brighten_rgb(uint32_t color, int amount) {
    uint16_t brightest_color = 0;
    uint16_t darkest_color = 255;

    for (int i = 0; i < 24; i += 8) {
        uint8_t c = ((color >> i) & 0xff);
        if (c > brightest_color) {
            brightest_color = c;
        }

        if (c < darkest_color) {
            darkest_color = c;
        }
    }

    double luminance = (
        (brightest_color + darkest_color) / 255.0
    ) / 2;

    double chroma = (
        brightest_color - darkest_color
    ) / 255.0 * 2;

    // Now that we have chroma and luminanace,
    // we can subtract the constant factor from each component
    // m = L - C / 2
    double constant = luminance - chroma / 2;

    // find the new constant
    luminance *= (1.0 + amount / 100.0);

    double adjusted = luminance - chroma / 2;

    for (int i = 0; i < 24; i += 8) {
        int16_t new_val = ((color >> i) & 0xff);

        new_val = (((new_val / 255.0) - constant) + adjusted) * 255.0;

        if (new_val > 255) {
            color |= (0xff << i);
        } else if (new_val < 0) {
            color &= ~(0xff << i);
        } else {
            color &= ~(0xff << i);
            color |= (new_val << i);
        }
    }

    return color;
}

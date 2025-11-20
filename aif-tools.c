#include "aif.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

// Takes in a RGB color and brightens it by the given percentage amount
uint32_t brighten_rgb(uint32_t color, int amount);

// Helper function to read a little-endian 16-bit value
uint16_t read_le16(FILE *fp) {
    uint8_t bytes[2];
    fread(bytes, 1, 2, fp);
    return bytes[0] | (bytes[1] << 8);
}

// Helper function to read a little-endian 32-bit value
uint32_t read_le32(FILE *fp) {
    uint8_t bytes[4];
    fread(bytes, 1, 4, fp);
    return bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}

void stage1_info(int n_files, const char **files) {
    for (int i = 0; i < n_files; i++) {
        const char *filename = files[i];

        // Get file size using stat
        struct stat file_stat;
        if (stat(filename, &file_stat) != 0) {
            fprintf(stderr, "Error: could not stat file %s\n", filename);
            exit(1);
        }
        long file_size = file_stat.st_size;

        // Open the file for reading
        FILE *fp = fopen(filename, "rb");
        if (fp == NULL) {
            fprintf(stderr, "Error: could not open file %s\n", filename);
            exit(1);
        }

        // Read magic number (4 bytes) - we're just skipping for stage 0
        uint8_t magic[4];
        fread(magic, 1, 4, fp);

        // Read checksum (2 bytes, little-endian)
        uint16_t checksum = read_le16(fp);
        uint8_t checksum_low = checksum & 0xff;
        uint8_t checksum_high = (checksum >> 8) & 0xff;

        // Read pixel format (1 byte)
        uint8_t pixel_format;
        fread(&pixel_format, 1, 1, fp);

        // Read compression (1 byte)
        uint8_t compression;
        fread(&compression, 1, 1, fp);

        // Read width (4 bytes, little-endian)
        uint32_t width = read_le32(fp);

        // Read height (4 bytes, little-endian)
        uint32_t height = read_le32(fp);

        // Read pixel data offset (4 bytes, little-endian) - not needed for stage 0
        // uint32_t pixel_offset = read_le32(fp);

        // Close the file
        fclose(fp);

        // Print the information
        printf("<%s>:\n", filename);
        printf("File-size: %ld bytes\n", file_size);
        printf("Checksum: %02x %02x\n", checksum_low, checksum_high);

        const char *format_name = aif_pixel_format_name(pixel_format);
        if (format_name != NULL) {
            printf("Pixel format: %s\n", format_name);
        } else {
            printf("Pixel format: Invalid\n");
        }

        const char *compression_name = aif_compression_name(compression);
        if (compression_name != NULL) {
            printf("Compression: %s\n", compression_name);
        } else {
            printf("Compression: Invalid\n");
        }

        printf("Width: %u px\n", width);
        printf("Height: %u px\n", height);
    }
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

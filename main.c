#include <stdio.h>
#include "common.c"
#include "fractran.c"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char* argv[]) {
	int width = 0;
	int height = 0;
	int* data = NULL;
    int ok = stbi_write_png("Fractran Spactime Diagram.png", width, height, 4, data, sizeof(*data) * width);
}
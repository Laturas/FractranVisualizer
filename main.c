#include <stdio.h>
#include "common.c"
#include "fractran.c"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int colors[11] = {
    0xffb8899c,  // 2
    0xffcaa6f0,  // 3
    0xffe6c3ef,  // 5
    0xffefe6f0,  // 7
    0xffddbeb8,  // 11
    0xff9c89ff,  // 13
    0xffffcc9c,  // 17
    0xff555555,  // 19
    0xffaaaaaa,  // 23
    0xffcccccc,  // 29
    0xffffffff, // 31
};

#define DEFAULT_STEPS 10000
#define DEFAULT_WIDTH 512
#define DEFAULT_HEIGHT 512

void create_fractran_spacetime_diagram(String machine, char* file_name, int steps, int img_width, int img_height) {
    ASSERT(img_width > 0);
    ASSERT(img_height > 0);

    if (steps <= 0) {
        steps = DEFAULT_STEPS;
    }

    Arena scratch_arena = {0};

    FractranProgram program = fractran_program_new(&scratch_arena, machine);
    if (program.vector_length > 11) {
        fprintf(stderr, "ERROR: Fractran Visualizer currently only supports up to 11 colors (highest prime = 31).\n");
        return;
    }
    FractranVector vec = fractran_init_vector(&scratch_arena, program.vector_length);

    int max_width = 1;

    for (int i = 0; i < steps; i++) {
        fractran_program_step(program, vec, &vec);
        int sum = 0;
        for (int j = 0; j < vec.length; j++) {
            sum += vec.values[j];
        }
        max_width = (sum > max_width) ? sum : max_width;
    }

    /* We now know the width and height */
    int steps_per_row = (steps + img_height - 1) / img_height;
    int steps_per_column = (max_width + img_width - 1) / img_width;
    ASSERT(steps_per_column > 0);

	int* data = malloc(sizeof(*data) * img_width * img_height);
    ASSERT(data != NULL);
    FractranVector current_step = fractran_init_vector(&scratch_arena, program.vector_length); /* Reset */

    int row_step = 0;
    int img_y = 0;
    
    for (int step = 0; step < steps; step++) {
        int img_x = 0;
        if (step % steps_per_row == 0) {
            int x = 0; /* program space */
            for (int i = 0; i < current_step.length; i++) {
                int stop_at = x + current_step.values[i];
                for (; x < stop_at; x++) {
                    if (x % steps_per_column == 0) {
                        data[(img_y * img_width) + img_x] = colors[i];
                        img_x++;
                    }
                }
            }
            for (; img_x < img_width; img_x++) {
                data[(img_y * img_width) + img_x] = 0xff000000;
            }
            img_y++;
        }
        bool is_halted = fractran_program_step(program, current_step, &current_step);
        if (is_halted) { break; }
    }

    for (; img_y < img_height; img_y++) {
        for (int img_x = 0; img_x < img_width; img_x++) {
            data[(img_y * img_width) + img_x] = 0xff000000;
        }
    }

    file_name = (file_name == NULL) ? "Fractran Spactime Diagram.png" : file_name;

    stbi_write_png(file_name, img_width, img_height, 4, data, sizeof(*data) * img_width);

    free(data);
    arena_free(&scratch_arena);
}

void print_help(char* argv_0) {
    printf("Fractran visualizer options:\n");
    printf("\tYou can pass in a single program by providing '<fractions>' as an argument. Example:\n");
    printf("\t\t%s '[63/10, 5/33, 2/3, 11/7, 105/2]'\n", argv_0);
    printf("\tYou can specify an output file name with the -o flag.\n");
    printf("\t\t%s '[63/10, 5/33, 2/3, 11/7, 105/2]' -o spacetime_diagram.png\n", argv_0);
    printf("\tYou can specify image dimensions with the -width and -height flags. The default is 512 for both otherwise.\n");
    printf("\t\t%s '[63/10, 5/33, 2/3, 11/7, 105/2]' -width 128 -height 512\n", argv_0);
    printf("\tYou can specify a step count with -s. Default step count is 10000\n");
    printf("\t\t%s '[63/10, 5/33, 2/3, 11/7, 105/2]' -s 1000\n", argv_0);
}

int main(int argc, char* argv[]) {
    char* input_str = NULL;
    char* output_file = NULL;
    char* steps_str = NULL;
    char* width_str = NULL;
    char* height_str = NULL;
    if (argc < 2) { goto ERR_USAGE; }

    for (int i = 1; i < argc; i++) {
        String current = string_null_to_length_terminated(argv[i]);

        String output_file_flag = (String) {.str = "-o", .length = 2};
        String step_count_flag = (String) {.str = "-s", .length = 2};
        String width_flag = (String) {.str = "-width", .length = 6};
        String height_flag = (String) {.str = "-height", .length = 7};
        String help_flag = (String) {.str = "-h", .length = 2};
        if (string_eq(current, output_file_flag)) {
            if (i + 1 >= argc) { goto ERR_USAGE; }

            output_file = argv[i + 1];
            i++;
        }
        else if (string_eq(current, step_count_flag)) {
            if (i + 1 >= argc) { goto ERR_USAGE; }

            steps_str = argv[i + 1];
            i++;
        }
        else if (string_eq(current, width_flag)) {
            if (i + 1 >= argc) { goto ERR_USAGE; }

            width_str = argv[i + 1];
            i++;
        }
        else if (string_eq(current, height_flag)) {
            if (i + 1 >= argc) { goto ERR_USAGE; }

            height_str = argv[i + 1];
            i++;
        }
        else if (string_starts_with(current, help_flag)) {
            print_help(argv[0]);
            return 0;
        }
        else {
            input_str = argv[i];
        }
    }

    if (input_str == NULL) { goto ERR_USAGE; }
    String input = string_null_to_length_terminated(input_str);

    int steps;
    if (steps_str == NULL) {
        steps = DEFAULT_STEPS;
    } else {
        String intstr = string_null_to_length_terminated(steps_str);
        if (!string_is_numeric(intstr)) { goto ERR_USAGE; }

        steps = string_to_int(intstr);
    }

    int width;
    if (width_str == NULL) {
        width = DEFAULT_WIDTH;
    } else {
        String widthstr = string_null_to_length_terminated(width_str);
        if (!string_is_numeric(widthstr)) { goto ERR_USAGE; }

        width = string_to_int(widthstr);
    }

    int height;
    if (height_str == NULL) {
        height = DEFAULT_HEIGHT;
    } else {
        String heightstr = string_null_to_length_terminated(height_str);
        if (!string_is_numeric(heightstr)) { goto ERR_USAGE; }

        height = string_to_int(heightstr);
    }

    create_fractran_spacetime_diagram(input, output_file, steps, width, height);

    return 0;


ERR_USAGE:
    fprintf(stderr, "Usage: %s '<fractran program>'\nAlternatively, use -help for more options.\n", argv[0]);
    return -1;
}
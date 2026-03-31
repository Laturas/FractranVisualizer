#include <stdio.h>
#include "common.c"
#include "fractran.c"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int colors[9] = {
    0xffb8899c,
    0xffcaa6f0,
    0xffe6c3ef,
    0xffefe6f0,
    0xffddbeb8,
    0xff555555,
    0xffaaaaaa,
    0xffcccccc,
    0xffffffff,
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
    int width = max_width;
    int height = steps;

	int* data = malloc(sizeof(*data) * max_width * steps);
    ASSERT(data != NULL);
    FractranVector current_step = fractran_init_vector(&scratch_arena, program.vector_length); /* Reset */

    /* We now run the program again, this time to fill in the image. */
    for (int y = 0; y < steps; y++) {
        int x = 0;
        for (int i = 0; i < current_step.length; i++) {
            int stop_at = x + current_step.values[i];
            for (; x < stop_at; x++) {
                data[(y * width) + x] = colors[i];
            }
        }
        for (; x < width; x++) {
            data[(y * width) + x] = 0xff000000;
        }
        fractran_program_step(program, current_step, &current_step);
    }

    file_name = (file_name == NULL) ? "Fractran Spactime Diagram.png" : file_name;

    stbi_write_png(file_name, width, height, 4, data, sizeof(*data) * width);

    free(data);
    arena_free(&scratch_arena);
}

void print_help(char* argv_0) {
    printf("Fractran visualizer options:\n");
    printf("\tYou can pass in a single program by providing '<fractions>' as an argument. Example:\n");
    printf("\t\t%s '[63/10, 5/33, 2/3, 11/7, 105/2]'\n", argv_0);
    printf("\tYou can specify an input file with the -i flag. Each program should be separated by a newline.\n");
    printf("\t\t%s -i input_programs.txt\n", argv_0);
    printf("\tYou can specify an output file name with the -o flag. If paired with an input file, each image will be named with format  \n");
    printf("\t\t%s -o input_programs.txt\n", argv_0);
}

int main(int argc, char* argv[]) {
    char* input_str = NULL;
    char* input_file = NULL;
    char* output_file = NULL;
    char* steps_str = NULL;
    char* width_str = NULL;
    char* height_str = NULL;

    for (int i = 1; i < argc; i++) {
        String current = string_null_to_length_terminated(argv[i]);

        String input_file_flag = (String) {.str = "-i", .length = 2};
        String output_file_flag = (String) {.str = "-o", .length = 2};
        String step_count_flag = (String) {.str = "-s", .length = 2};
        String width_flag = (String) {.str = "-w", .length = 2};
        String height_flag = (String) {.str = "-h", .length = 2};

        if (current.str[0] == '-' && current.str[1] == 'h') {
            print_help(argv[0]);
            return 0;
        }
        else if (string_eq(current, input_file_flag)) {
            if (i + 1 >= argc) { goto ERR_USAGE; }

            input_file = argv[i + 1];
            i++;
        }
        else if (string_eq(current, output_file_flag)) {
            if (i + 1 >= argc) { goto ERR_USAGE; }

            output_file = argv[i + 1];
            i++;
        }
        else if (string_starts_with(current, step_count_flag)) {
            if (i + 1 >= argc) { goto ERR_USAGE; }

            steps_str = argv[i + 1];
            i++;
        }
        else if (string_starts_with(current, width_flag)) {
            if (i + 1 >= argc) { goto ERR_USAGE; }

            width_str = argv[i + 1];
            i++;
        }
        else {
            input_str = argv[i];
        }
    }

    // char* input_str = argv[1];
    String input = string_null_to_length_terminated(input_str);

    int steps;
    if (steps_str == NULL) {
        steps = DEFAULT_STEPS;
    } else {
        String intstr = string_null_to_length_terminated(steps_str);
        if (!string_is_numeric(intstr)) { goto ERR_USAGE; }

        steps = string_to_int(intstr);
    }

    create_fractran_spacetime_diagram(input, "Fractran Spacetime Diagram", steps, DEFAULT_WIDTH, DEFAULT_HEIGHT);

    return 0;


ERR_USAGE:
    fprintf(stderr, "Usage: %s '<fractran program>'\nAlternatively, use -help for more options.\n", argv[0]);
    return -1;
}
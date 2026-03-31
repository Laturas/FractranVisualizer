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

void create_fractran_spacetime_diagram(String machine, char* file_name) {

    Arena scratch_arena = {0};

    FractranProgram program = fractran_program_new(&scratch_arena, machine);

    int steps = 2048;
    FractranVector* step_outputs = arena_alloc(&scratch_arena, sizeof(*step_outputs) * steps);
    for (int i = 0; i < steps; i++) {
        step_outputs[i] = (FractranVector) {
            .length = program.vector_length,
            .values = arena_alloc(&scratch_arena, sizeof(*step_outputs[0].values) * program.vector_length)
        };

        for (int j = 0; j < program.vector_length; j++) {
            step_outputs[i].values[j] = 0;
        }
    }

    step_outputs[0].values[0] = 1;

    for (int i = 1; i < steps; i++) {
        step_outputs[i] = (FractranVector) {
            .length = program.vector_length,
            .values = arena_alloc(&scratch_arena, sizeof(*step_outputs[i].values) * program.vector_length)
        };

        bool is_halted = fractran_program_step(program, step_outputs[i - 1], &step_outputs[i]);
        if (is_halted) {
            break;
        }
    }

    int max_value = 0;

    for (int i = 0; i < steps; i++) {
        int width_sum = 0;
        for (int j = 0; j < step_outputs[i].length; j++) {
            width_sum += step_outputs[i].values[j];
        }
        max_value = (max_value < width_sum) ? width_sum : max_value;
    }

	int width = max_value;
	int height = steps;
	int* data = malloc(sizeof(*data) * width * height);

    for (int y = 0; y < height; y++) {
        int x = 0;
        FractranVector current_step = step_outputs[y];
        for (int i = 0; i < current_step.length; i++) {
            int stop_at = x + current_step.values[i];
            for (; x < stop_at; x++) {
                data[(y * width) + x] = colors[i];
            }
        }
        for (; x < width; x++) {
            data[(y * width) + x] = 0xff000000;
        }
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
    printf("\t\t%s -i input_programs.txt\n", argv_0);
}

int main(int argc, char* argv[]) {
    char* input_str = NULL;
    char* input_file = NULL;
    char* output_file = NULL;

    for (int i = 1; i < argc; i++) {
        String current = string_null_to_length_terminated(argv[i]);
        String input_file_flag = (String) {.str = "-i", .length = 2};
        String output_file_flag = (String) {.str = "-o", .length = 2};

        if (current.str[0] == '-' && current.str[1] == 'h') {
            print_help(argv[0]);
            return 0;
        }

        if (string_eq(current, input_file_flag)) {
            if (i + 1 >= argc) { goto ERR_USAGE; }

            input_file = current.str;
        }

        if (string_eq(current, output_file_flag)) {
            if (i + 1 >= argc) { goto ERR_USAGE; }

            output_file = current.str;
        }
    }

    // char* input_str = argv[1];
    String input = string_null_to_length_terminated(input_str);

    create_fractran_spacetime_diagram(input, "Fractran Spacetime Diagram");

    return 0;


ERR_USAGE:
    fprintf(stderr, "Usage: %s '<fractran program>'\nAlternatively, use -help for more options.\n", argv[0]);
    return -1;
}
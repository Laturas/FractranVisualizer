/**
* Katelyn's argument parsing library
*/
#ifndef COMMON
    #include "common.c"
#endif

typedef struct ArgBuilder {
    Arena* arena;
    int a;
} ArgBuilder;

ArgBuilder args_new(Arena* args_arena) {

}


/**
    char* input_file_name;
    char* output_file_name;

    ArgBuilder builder = args_new(&args_arena);

    ArgFlag input_file = args_flag_create_required(&builder, "-input", &input_file_name);
    ArgFlag output_file = args_flag_create_required(&builder, "-output", &output_file_name);

    args_evaluate(&builder)
*/
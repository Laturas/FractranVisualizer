#ifndef COMMON
    #include "common.c"
#endif



typedef struct FractranVector {
    int length;
    int* values;
} FractranVector;

typedef struct FractranProgram {
    int vector_length;
    int vector_count;
    FractranVector* vectors;
} FractranProgram;

#define IS_NUMERIC(chr) ('0' <= (chr) && (chr) <= '9')

/**
* Returns how long a fractran program is.
* Attempts to give an answer no matter what. This makes it very flexible as to the format it can parse,
* but don't give it an evil format.
* 
* THIS FUNCTION IS NOT A VALIDATOR
*/
int fractran_program_get_length(String program_string) {
    typedef enum NumState {
        NUMSTATE_NONE,
        NUMSTATE_FIRST_NUMBER,
        NUMSTATE_DIVISION_SYMBOL,
        NUMSTATE_POST_DIVISION_SYMBOL,
        NUMSTATE_SECOND_NUMBER,
    } NumState;

    int count = 0;
    NumState current_state = NUMSTATE_NONE;

    for (int i = 0; i < program_string.length; i++) {
        switch (current_state) {
            case NUMSTATE_NONE: {
                if (IS_NUMERIC(program_string.str[i])) {
                    current_state = NUMSTATE_FIRST_NUMBER;
                }
            } break;
            case NUMSTATE_FIRST_NUMBER: {
                if (!IS_NUMERIC(program_string.str[i])) {
                    current_state = (program_string.str[i] == '/') ? NUMSTATE_POST_DIVISION_SYMBOL : NUMSTATE_DIVISION_SYMBOL;
                }
            } break;
            case NUMSTATE_DIVISION_SYMBOL: {
                if ((program_string.str[i]) == '/') {
                    current_state = NUMSTATE_POST_DIVISION_SYMBOL;
                }
            } break;
            case NUMSTATE_POST_DIVISION_SYMBOL: {
                if (IS_NUMERIC(program_string.str[i])) {
                    count++;
                    current_state = NUMSTATE_SECOND_NUMBER;
                }
            } break;
            case NUMSTATE_SECOND_NUMBER: {
                if (!IS_NUMERIC(program_string.str[i])) {
                    current_state = NUMSTATE_NONE;
                }
            } break;
        }
    }
    return count;
}

int fractran_program_get_vector_length(String program_string) {

}

/**
* Constructs a fractran program from a list:
* Ex: 1/12, 75/7, 49/33, 2/5, 77/2
*
* Can be separated via any characters that aren't numbers or slashes
*/
FractranProgram fractran_program_new(Arena* fractran_arena, String program_string) {
    int program_len = fractran_program_get_length(program_string);
    FractranProgram program = {0};

    program.vector_count = program_len;

    int saved = arena_save(fractran_arena);
    program.vectors = arena_alloc(fractran_arena, sizeof(*program.vectors) * program_len);


    
    return program;
}
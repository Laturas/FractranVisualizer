#ifndef COMMON
    #include "common.c"
#endif

#include "primes.h"

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
	typedef enum NumState {
        NUMSTATE_NONE,
        NUMSTATE_FIRST_NUMBER,
        NUMSTATE_DIVISION_SYMBOL,
        NUMSTATE_POST_DIVISION_SYMBOL,
        NUMSTATE_SECOND_NUMBER,
    } NumState;

    int max_length = 0;
    NumState current_state = NUMSTATE_NONE;

    for (int i = 0; i < program_string.length; i++) {
        switch (current_state) {
            case NUMSTATE_NONE: {
                if (IS_NUMERIC(program_string.str[i])) {
					int value = atoi(&program_string.str[i]);
					if (value < 1) {
						return 0;
					}

					int largest_prime_index = -1;
					for (int i = 0; (i < 255) && (value > 1); i++) {
						while (value % primes[i] == 0) {
							largest_prime_index = i;
							value /= primes[i];
						}
					}

					int new_length = largest_prime_index + 1;

					max_length = (new_length > max_length) ? new_length : max_length;
                    current_state = NUMSTATE_FIRST_NUMBER;
					while (IS_NUMERIC(program_string.str[i])) {
						i++;
					}
					i--;
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
					int value = atoi(&program_string.str[i]);
					if (value < 1) {
						return 0;
					}

					int largest_prime_index = -1;
					for (int i = 0; (i < 255) && (value > 1); i++) {
						while (value % primes[i] == 0) {
							largest_prime_index = i;
							value /= primes[i];
						}
					}

					int new_length = largest_prime_index + 1;

					max_length = (new_length > max_length) ? new_length : max_length;
                    current_state = NUMSTATE_SECOND_NUMBER;
					while (IS_NUMERIC(program_string.str[i])) {
						i++;
					}
					i--;
                }
            } break;
            case NUMSTATE_SECOND_NUMBER: {
                if (!IS_NUMERIC(program_string.str[i])) {
                    current_state = NUMSTATE_NONE;
                }
            } break;
        }
    }
    return max_length;
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
	int vector_length = fractran_program_get_vector_length(program_string);
	program.vector_length = vector_length;

	typedef enum NumState {
        NUMSTATE_NONE,
        NUMSTATE_FIRST_NUMBER,
        NUMSTATE_DIVISION_SYMBOL,
        NUMSTATE_POST_DIVISION_SYMBOL,
        NUMSTATE_SECOND_NUMBER,
    } NumState;

	NumState current_state = NUMSTATE_NONE;
	int current_vector = 0;
	int j = 0;

	for (int i = 0; i < program_len; i++) {
		program.vectors[i] = (FractranVector) {
			.length = vector_length,
			.values = arena_alloc(fractran_arena, sizeof(*program.vectors[i].values) * vector_length)
		};
		ASSERT(j < program_string.length);
		while (!IS_NUMERIC(program_string.str[j])) {
			j++;
			ASSERT(j < program_string.length);
		}

		ASSERT(j < program_string.length);

		int first_int = atoi(&program_string.str[j]);

		for (int k = 0; (k < vector_length) && (first_int > 1); k++) {
			while (first_int % primes[k] == 0) {
				program.vectors[i].values[k] += 1;
				first_int /= primes[k];
			}
		}
		ASSERT(first_int == 1);

		while (IS_NUMERIC(program_string.str[j])) {
			j++;
			ASSERT(j < program_string.length);
		}

		ASSERT(j < program_string.length);
		while (!IS_NUMERIC(program_string.str[j])) {
			j++;
			ASSERT(j < program_string.length);
		}


		int second_int = atoi(&program_string.str[j]);

		for (int k = 0; (k < vector_length) && (second_int > 1); k++) {
			while (second_int % primes[k] == 0) {
				program.vectors[i].values[k] -= 1;
				second_int /= primes[k];
			}
		}
		ASSERT(second_int == 1);

		while (IS_NUMERIC(program_string.str[j])) {
			j++;
		}
	}
    
    return program;
}

/**
* Returns a fractran vector of the given length useable for starting a run from blank (first element is 1, the rest are 0)
*/
FractranVector fractran_init_vector(Arena* fractran_arena, int length) {
    int* values = arena_alloc(fractran_arena, length * sizeof(*values));

    /* Optimized to a memcpy */
    for (int i = 0; i < length; i++) {
        values[i] = 0;
    }
    values[0] = 1;

    return (FractranVector) {
        .length = length,
        .values = values
    };
}

bool fractran_program_step(FractranProgram program, FractranVector input_vector, FractranVector* output_vector) {
    ASSERT(output_vector != NULL);
    ASSERT(input_vector.length == program.vector_length);

    int rule = 0;
    for (rule = 0; rule < program.vector_count; rule++) {
        bool is_valid = true;
        for (int i = 0; i < program.vector_length; i++) {
            if (program.vectors[rule].values[i] < 0 && input_vector.values[i] + program.vectors[rule].values[i] < 0) {
                is_valid = false;
                break;
            }
        }
        if (is_valid) { break; }
    }
    if (rule >= program.vector_count) { return true; } /* Machine is already halted */

    for (int i = 0; i < program.vector_length; i++) {
        output_vector->values[i] = input_vector.values[i] + program.vectors[rule].values[i];
    }

    return false;
}
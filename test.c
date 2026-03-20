#include <stdio.h>
#include "common.c"
#include "fractran.c"

int main(int argc, char* argv[]) {
    char* first = "[1/12, 75/7, 49/33, 2/5, 77/2]";
    char* second = "9/10 4/21 77/2 5/7 14/11";
    char* third_weird_one = "9/10,  4/21 77/2 \n 5/7 [14 / 11]";

    String first_string = string_null_to_length_terminated(first);
    String second_string = string_null_to_length_terminated(second);
    String weird_string = string_null_to_length_terminated(third_weird_one);

    ASSERT(fractran_program_get_length(first_string) == 5);
    ASSERT(fractran_program_get_length(second_string) == 5);
    ASSERT(fractran_program_get_length(weird_string) == 5);
	
	ASSERT(fractran_program_get_vector_length(first_string) == 5);
	ASSERT(fractran_program_get_vector_length(second_string) == 5);
	ASSERT(fractran_program_get_vector_length(weird_string) == 5);

	Arena fractran_arena = {0};

	char* bbf15_champ = "1/45, 4/5, 3/2, 25/3";

	String bbf15_string = string_null_to_length_terminated(bbf15_champ);

	FractranProgram program = fractran_program_new(&fractran_arena, bbf15_string);
	printf("%d, %d\n", program.vector_count, program.vector_length);

	for (int i = 0; i < program.vector_count; i++) {
		printf("%d", program.vectors[i].values[0]);
		for (int j = 1; j < program.vector_length; j++) {
			printf(", %d", program.vectors[i].values[j]);
		}
		printf("\n");
	}

	program = fractran_program_new(&fractran_arena, (String) {.str = "[7/15, 22/3, 6/77, 5/2, 9/5]", .length = sizeof("[7/15, 22/3, 6/77, 5/2, 9/5]") - 1});

	for (int i = 0; i < program.vector_count; i++) {
		printf("%d", program.vectors[i].values[0]);
		for (int j = 1; j < program.vector_length; j++) {
			printf(", %d", program.vectors[i].values[j]);
		}
		printf("\n");
	}
}
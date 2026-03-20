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
}
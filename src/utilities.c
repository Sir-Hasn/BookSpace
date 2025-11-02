#include "headers/main.h"
#include "headers/database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
} // discarding characters until a newline (\n) or end-of-file (EOF) is encountered


int str_equals_ignore_case(const char* str1, const char* str2) {
    while (*str1 && *str2) {
        if (tolower((unsigned char)*str1) != tolower((unsigned char)*str2)) {
            return 0;
        }
        str1++;
        str2++;
    }
    return *str1 == *str2;
} //  compares two strings case-insensitively, returning 1 (true) if they match and 0 (false) otherwise. It iterates through characters, converting to lowercase for comparison.


int check_cancel_string(const char* input) {
    char temp[100];
    strncpy(temp, input, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    
    for(int i = 0; temp[i]; i++){
        temp[i] = tolower(temp[i]);
    }
    
    return strcmp(temp, "cancel") == 0;
} //  checks if the input string equals "cancel" (case-insensitively), returning 1 if true and 0 otherwise. It creates a lowercase copy of the input and compares it
#ifndef MAIN_H
#define MAIN_H

#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Const Declarations
#define MAX_NAME_LENGTH 100
#define MAX_DATE_LENGTH 11
#define MAX_TIME_LENGTH 10
#define DB_PATH "data/library_reservations.db"

//Func Declarations
int main();
void main_menu();
void view_daily_schedule();
void make_reservation();
void cancel_reservation();
void search_reservations();
void edit_reservation();
void exit_program();
void clear_screen();
void pause_screen();

// Utils
void clear_input_buffer();
void generate_reservation_id(char* out_id, size_t out_size, const char* reservation_date);
int check_cancel_string(const char* input);

// Missing declarations
int validate_date(const char* date);
int validate_date_for_viewing(const char* date);
int validate_time(const char* time);
int validate_time_range(const char* start_time, const char* end_time);
void format_time_24hour(char* time_12, char* time_24);
#endif // MAIN_H
#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>

//DB Functions Declarations
int initialize_database();
int create_tables();
int close_database();
int check_time_conflict(const char* date, const char* start_time, const char* end_time, const char* consultation_room);
int insert_reservation(const char* name, const char* student_num, const char* date, const char* start_time, const char* end_time, const char* reservation_id, const char* consultation_room);
int delete_reservation(const char* reservation_id);
int reservation_exists(const char* reservation_id);
int get_reservations_by_date(const char* date);
int get_reservations_by_student_num(const char* student_num);
int get_reservations_by_id(const char* reservation_id);
int get_all_reservations();
int update_reservation(const char* reservation_id, const char* name, const char* date, const char* start_time, const char* end_time, const char* student_num, const char* consultation_room);
int get_reservations_by_name(const char* name);


//Callback functions
int callback_print_reservations(void* data, int argc, char** argv, char** azColName);
int callback_count_reservations(void* data, int argc, char** argv, char** azColName);


#endif // DATABASE_H
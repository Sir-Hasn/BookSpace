#include "headers/database.h"
#include "headers/reservation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//database connection
extern sqlite3* db;

//Const
#define MAX_NAME_LENGTH 100
#define MAX_DATE_LENGTH 11
#define MAX_TIME_LENGTH 10
#define DATABASE_PATH "data/library_reservations.db"

int close_database() {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
    return 0; 
}

int initialize_database() {
    // Open database connection
    int rc;

    rc = sqlite3_open(DATABASE_PATH, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // Create tables if they don't exist
    if (create_tables() != 0) {
        fprintf(stderr, "Failed to create tables.\n");
        sqlite3_close(db);
        return 1;
    }
    return 0;
}

int create_tables() {
    char* sql = "CREATE TABLE IF NOT EXISTS reservations ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "student_name TEXT NOT NULL,"
                "student_num TEXT NOT NULL,"
                "date TEXT NOT NULL,"
                "start_time TEXT NOT NULL,"
                "end_time TEXT NOT NULL,"
                "reservation_id TEXT NOT NULL UNIQUE,"
                "consultation_room TEXT NOT NULL,"
                "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
                "UNIQUE(date, start_time, end_time, consultation_room)"
                ");";
    char* err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }
    return 0;
}


int check_time_conflict(const char* date, const char* start_time, const char* end_time, const char* consultation_room) {
    char sql[600];
    char* err_msg = 0;
    int conflict = 0;

    // convert to 24-hour format
    char start_24[MAX_TIME_LENGTH], end_24[MAX_TIME_LENGTH];
    format_time_24hour((char*)start_time, start_24);
    format_time_24hour((char*)end_time, end_24);

    // Only check conflicts within the SAME room
    sprintf(sql,
        "SELECT COUNT(*) FROM reservations "
        "WHERE date = '%s' AND consultation_room = '%s' AND ("
        "   (start_time < '%s' AND end_time > '%s') OR "
        "   (start_time >= '%s' AND start_time < '%s') OR "
        "   (end_time > '%s' AND end_time <= '%s')"
        ");",
        date, consultation_room, end_24, start_24, start_24, end_24, start_24, end_24);

    int rc = sqlite3_exec(db, sql, callback_count_reservations, &conflict, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1; // error
    }

    return conflict > 0 ? 1 : 0; // 1 = conflict, 0 = available
}


int insert_reservation(const char* name, const char* student_num, const char* date, const char* start_time, const char* end_time, const char* reservation_id, const char* consultation_room) {
    char* err_msg = 0;
    char sql[600];

    // convert to 24-hour format
    char start_24[MAX_TIME_LENGTH], end_24[MAX_TIME_LENGTH];
    format_time_24hour((char*)start_time, start_24);
    format_time_24hour((char*)end_time, end_24);

    //CHECK FOR CONFLICT BEFORE INSERTING
    if (check_time_conflict(date, start_24, end_24, consultation_room)) {
        fprintf(stderr, "Reservation conflict detected. Room %s is already booked on %s between %s and %s.\n", consultation_room, date, start_time, end_time);
        return 1;
    }

    //INSERT RESERVATION IF NO CONFLICT
    sprintf(sql, "INSERT INTO reservations (student_name, student_num, date, start_time, end_time, reservation_id, consultation_room) "
                 "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s');",
                 name, student_num, date, start_24, end_24, reservation_id, consultation_room);

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }

    return 0;
}

int delete_reservation(const char* reservation_id) {
        char sql[200];
        char* err_msg = 0;

        sprintf(sql, "DELETE FROM reservations WHERE reservation_id = '%s';", reservation_id);
        //printf("DEBUG - Executing SQL: %s\n", sql);
       //printf("DEBUG - Reservation ID length: %zu\n", strlen(reservation_id));

        int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", err_msg);
            sqlite3_free(err_msg);
            return -1;
        }

        int changes = sqlite3_changes(db);
        
        if (changes == 0) {
            return 1;
        }

        return 0;
    }

    int reservation_exists(const char* reservation_id) {
    char sql[200];
    char* err_msg = 0;
    int count = 0;

    sprintf(sql, "SELECT COUNT(*) FROM reservations WHERE reservation_id = '%s';", reservation_id);
    
    int rc = sqlite3_exec(db, sql, callback_count_reservations, &count, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1; // Error
    }
    
    return count > 0 ? 1 : 0; // 1 = exists, 0 = doesn't exist
}

int update_reservation(const char* reservation_id, const char* name, const char* student_num, 
                       const char* date, const char* start_time, const char* end_time, 
                       const char* consultation_room) {
    char sql[600];
    char* err_msg = 0;

    // Convert time inputs to 24-hour format
    char start_24[MAX_TIME_LENGTH], end_24[MAX_TIME_LENGTH];
    format_time_24hour((char*)start_time, start_24);
    format_time_24hour((char*)end_time, end_24);

    // Check for time conflicts before updating
    if (check_time_conflict(date, start_24, end_24, consultation_room)) {
        fprintf(stderr, "Time conflict detected. Room %s is already booked on %s between %s and %s.\n",
                consultation_room, date, start_time, end_time);
        return 1;
    }

    // Prepare SQL update query
    sprintf(sql,
        "UPDATE reservations SET "
        "student_name = '%s', "
        "student_num = '%s', "
        "date = '%s', "
        "start_time = '%s', "
        "end_time = '%s', "
        "consultation_room = '%s' "
        "WHERE reservation_id = '%s';",
        name, student_num, date, start_24, end_24, consultation_room, reservation_id);

    // Execute update query
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error while updating reservation: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }

    printf("Reservation [%s] updated successfully.\n", reservation_id);
    return 0;
}

int get_reservations_by_date(const char* date) {
    char sql[200];
    char* err_msg = 0;

    sprintf(sql, "SELECT * FROM reservations WHERE date = '%s' ORDER BY start_time;", date);

    int rc = sqlite3_exec(db, sql, callback_print_reservations, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }

    return 0;
}

int get_reservations_by_student_num(const char* student_num) {
    char sql[200];
    char* err_msg = 0;

    sprintf(sql, "SELECT * FROM reservations WHERE student_num = '%s' ORDER BY date, start_time;", student_num);

    int rc = sqlite3_exec(db, sql, callback_print_reservations, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }

    return 0;
}

int get_reservations_by_id(const char* reservation_id) {
    char sql[200];
    char* err_msg = 0;

    sprintf(sql, "SELECT * FROM reservations WHERE reservation_id = '%s';", reservation_id);

    int rc = sqlite3_exec(db, sql, callback_print_reservations, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }

    return 0;
}

int get_all_reservations() {
    char* sql = "SELECT * FROM reservations ORDER BY date, start_time;";
    char* err_msg = 0;

    int rc = sqlite3_exec(db, sql, callback_print_reservations, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }

    return 0;
}

int get_reservations_by_name(const char* name) {
    char sql[300];
    char* err_msg = 0;

    sprintf(sql, "SELECT * FROM reservations WHERE student_name LIKE '%%%s%%' ORDER BY date, start_time;", name);

    int rc = sqlite3_exec(db, sql, callback_print_reservations, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }

    return 0;
}

// Callback implementations
int callback_print_reservations(void* data, int argc, char** argv, char** azColName) {
    (void)data;
    (void)azColName;
    
    // Format: reservation_id | start_time | end_time | student_name
    // Column order: id(0), student_name(1), student_num(2), date(3), start_time(4), end_time(5), reservation_id(6), consultation_room(7), created_at(8)
    if (argc >= 8) {
        const char* reservation_id = argv[6] ? argv[6] : "NULL";
        const char* date = argv[3] ? argv[3] : "NULL";
        const char* student_name = argv[1] ? argv[1] : "NULL";
        const char* consultation_room = argv[7] ? argv[7] : "NULL";
        const char* start_time = argv[4] ? argv[4] : "NULL";
        const char* end_time = argv[5] ? argv[5] : "NULL";
        
        // Convert 24-hour times back to 12-hour format for display
        char start_12[MAX_TIME_LENGTH], end_12[MAX_TIME_LENGTH];
        format_time_12hour((char*)start_time, start_12);
        format_time_12hour((char*)end_time, end_12);
        
        printf("| %-20s | %-10s | %-15s | %-10s | %-10s | %-25s |\n", 
               reservation_id, date, consultation_room, start_12, end_12, student_name);
    }
    return 0;
}

int callback_count_reservations(void* data, int argc, char** argv, char** azColName) {
    (void)azColName;
    if (argc > 0 && argv && argv[0]) {
        int* out = (int*)data;
        *out = atoi(argv[0]);
    }
    return 0;
}
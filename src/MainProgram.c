#include "headers/main.h"
#include "headers/database.h"
#include "headers/reservation.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

//DATABASE CONNECTION
sqlite3 *db;

//LAGAY SA UTILS
void to_uppercase (char *str) {
    for (int i = 0; str[i]; i++){
        str[i] =toupper (str[i]);
    }
}

int main() {
    printf("LIBRARY CONSULTATION ROOM RESERVATION SYSTEM\n");
    printf("Initializing system...\n\n");

    //Initialize db
    if (initialize_database() != 0) {
        printf("Error: Failed to initialize database. Exiting...\n");
        pause_screen();
        return 1;
    }

    //Main program
    main_menu();

    //Close db
    close_database();
    
    return 0;
}

void main_menu(){
    int choice;
     do {
        clear_screen();
        printf("LIBRARY CONSULTATION ROOM RESERVATION SYSTEM\n");
        printf("--------------------------------------------\n");
        printf("\n");
        printf("Main Menu:\n");
        printf("1. View Daily Schedule\n");
        printf("2. Make a Reservation\n");
        printf("3. Cancel a Reservation\n");
        printf("4. Edit Reservation\n");
        printf("5. Search Reservations\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");


        if(scanf("%d", &choice) != 1){

            printf("Invalid input. Please enter a number between 1 and 6.\n");
            clear_input_buffer();
            pause_screen();
            continue;
        } //INPUT != VALID, CLEAR BUFFER AND ASK AGAIN

        switch (choice) {
            case 1:
                view_daily_schedule();
                break;
            case 2:
                make_reservation();
                break;
            case 3:
                cancel_reservation();
                break;
            case 4:
                edit_reservation();
                pause_screen();
                break;
            case 5:
                search_reservations();
                break;
            case 6:
                exit_program();
                break;
            default:
                clear_input_buffer();
                printf("Invalid choice. Please try again.\n");
                pause_screen();
        }
     } while (choice != 6);
}

void view_daily_schedule() {
    char date[MAX_DATE_LENGTH];
    int valid_input = 0;

    do { //main loop
        clear_screen();
        printf("VIEW DAILY SCHEDULE\n");
        printf("-------------------\n");
        printf("Enter date (MM/DD/YYYY) ['cancel' to abort]: ");

        if(scanf("%s", date) != 1){
            printf("Error reading date input.\n");
            clear_input_buffer();
            pause_screen();
            continue;  // ask again
        }

            // allow user to cancel
            char temp[30];
            strcpy(temp, date);
            for(int i = 0; temp[i]; i++){
                temp[i] = tolower(temp[i]);
            }
            
            if (strcmp(temp, "cancel") == 0) {
                clear_input_buffer();
                printf("Operation cancelled.\n");
                pause_screen();
                valid_input = 1; // exit inner loop
                break;
            }

        clear_input_buffer();  // clear buffer after successful scan

        // validate date
        if (!validate_date_for_viewing(date)) {
            printf("Invalid date format. Please use MM/DD/YYYY.\n");
            pause_screen();
            continue;  // ask again
        }

        valid_input = 1;  // input = valid, exit loop

     // display sched
    printf("\nSchedule for %s\n", date);
    printf("+----------------------+------------+-----------------+------------+------------+---------------------------+\n");
    printf("| %-20s | %-10s | %-15s | %-10s | %-10s | %-25s |\n",
        "Reservation ID", "Date", "Room", "Start Time", "End Time", "Student Name");
    printf("+----------------------+------------+-----------------+------------+------------+---------------------------+\n");

    // get reservations for the date
    int result = get_reservations_by_date(date);

    if (result != 0) {
        printf("| %-112s |\n", "Error retrieving schedule data.");
    }

    printf("+----------------------+------------+-----------------+------------+------------+---------------------------+\n");
    pause_screen();
    } while (!valid_input);
}

void make_reservation(){
    char student_name[MAX_NAME_LENGTH];
    char student_num[MAX_STUD_ID_LENGTH];
    char date[MAX_DATE_LENGTH];
    char start_time[MAX_TIME_LENGTH];
    char end_time[MAX_TIME_LENGTH];
    char reservation_id[MAX_RESERVATION_ID_LENGTH];
    char consultation_room[MAX_ROOM_LENGTH];
    char input_buffer[50];

    // Step 1: Consultation room
    int room_choice;
    int valid = 0;
    do {
        clear_screen();
        printf("MAKE A RESERVATION\n");
        printf("-------------------\n\n");
   
        printf("Available Consultation Rooms:\n");
        for (int i = 0; i < NUM_ROOMS; i++) {
            printf("%d. %s\n", i + 1, CONSULTATION_ROOMS[i]);
        }
        printf("Select consultation room ('cancel' to abort): ");

        if(scanf("%s", input_buffer) != 1){
            printf("Invalid choice. Please try again.\n");
            pause_screen();
            continue;
        }
        clear_input_buffer();

        if (check_cancel_string(input_buffer)) {
            printf("Reservation cancelled.\n");
            pause_screen();
            return;
        }

        room_choice = atoi(input_buffer);
        if (room_choice < 1 || room_choice > NUM_ROOMS) {
            printf("Invalid choice. Please try again.\n");
            pause_screen();
            continue;
        }

        strcpy(consultation_room, CONSULTATION_ROOMS[room_choice - 1]);
        valid = 1;
    } while (!valid);

    // Step 2: Student name
    valid = 0;
    do {
        clear_screen();
        printf("MAKE A RESERVATION\n");
        printf("-------------------\n\n");
        printf("Selected Room: %s\n\n", consultation_room);
        printf("Enter student name (or 'cancel' to abort): ");
        
        if(fgets(student_name, sizeof(student_name), stdin) == NULL){
            printf("Error reading input. Please try again.\n");
            pause_screen();
            continue;
        }
        student_name[strcspn(student_name, "\n")] = 0;
        
        if (check_cancel_string(student_name)) {
            printf("Reservation cancelled.\n");
            pause_screen();
            return;
        }
        
        if (strlen(student_name) == 0) {
            printf("Name cannot be empty. Please try again.\n");
            pause_screen();
            continue;
        }
        valid = 1;
    } while (!valid);

    // Step 3: Student number
    valid = 0;
    do {
        clear_screen();
        printf("MAKE A RESERVATION\n");
        printf("-------------------\n\n");
        printf("Selected Room: %s\n", consultation_room);
        printf("Student Name: %s\n\n", student_name);
        printf("Enter student number ('cancel' to abort): ");
        
        if (scanf("%s", student_num) != 1) {
            clear_input_buffer();
            printf("Error reading input. Please try again.\n");
            pause_screen();
            continue;
        }
        clear_input_buffer();
        
        if (check_cancel_string(student_num)) {
            printf("Reservation cancelled.\n");
            pause_screen();
            return;
        }
        
        if (strlen(student_num) != 10 || 
            strspn(student_num, "0123456789") != 8 || 
            student_num[8] != '-' || 
            !isalpha((unsigned char)student_num[9])) {// ALPHABET CHECK
            printf("Invalid student number.\n");
            pause_screen();
            continue;
        } 
        valid = 1;
    } while (!valid);

    // Step 4: Get month
    int month, day, year, mday;
    valid = 0;
    do {
        clear_screen();
        printf("MAKE A RESERVATION\n");
        printf("-------------------\n\n");
        printf("Selected Room: %s\n", consultation_room);
        printf("Student Name: %s\n", student_name);
        printf("Student Number: %s\n\n", student_num);
        
        printf("Select month:\n");
        printf("1. January    2. February   3. March      4. April\n");
        printf("5. May        6. June       7. July       8. August\n");
        printf("9. September  10. October   11. November  12. December\n");
        printf("Enter month (1-12, or 'cancel' to abort): ");
        
        if (scanf("%s", input_buffer) != 1) {
            clear_input_buffer();
            printf("Invalid month. Please try again.\n");
            pause_screen();
            continue;
        }
        clear_input_buffer();
        
        if (check_cancel_string(input_buffer)) {
            printf("Reservation cancelled.\n");
            pause_screen();
            return;
        }
    
        time_t t = time(NULL); // Grabs current time and stores it in variable t
        struct tm *today = localtime(&t); // Converts t to localtime as structure broken down to year month day
        int current_month = today->tm_mon + 1; // Months start at 0 (= January), and +1 is added so 1 = January
        
        month = atoi(input_buffer);
        if (month < current_month) {
            printf("Invalid month. Please try again.\n");
            pause_screen();
            continue;
        }
        
        switch (month) {
            case 1: case 3: case 5: case 7: case 8: case 10: case 12: 
                mday = 31; break;
            case 4: case 6: case 9: case 11: 
                mday = 30; break;
            case 2: 
                mday = 29; break;
        }
        valid = 1;
    } while (!valid);

    // Step 5: Get day
    valid = 0;
    do {
        clear_screen();
        printf("MAKE A RESERVATION\n");
        printf("-------------------\n\n");
        printf("Selected Room: %s\n", consultation_room);
        printf("Student Name: %s\n", student_name);
        printf("Student Number: %s\n", student_num);
        printf("Month: %d\n\n", month);
        
        printf("Enter day (1-%d, or 'cancel' to abort): ", mday);
        
        if (scanf("%s", input_buffer) != 1) {
            clear_input_buffer();
            printf("Invalid input. Please try again.\n");
            pause_screen();
            continue;
        }
        clear_input_buffer();
        
        if (check_cancel_string(input_buffer)) {
            printf("Reservation cancelled.\n");
            pause_screen();
            return;
        }

        day = atoi(input_buffer);
        if(day < 1 || day > mday) {
            printf("Invalid day for chosen month. Please try again.\n");
            pause_screen();
            continue;
        }
        valid = 1;
    } while (!valid);

    // Auto-set year
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    year = t->tm_year + 1900;
    printf("\nYear has been automatically set to %d.\n", year);

    snprintf(date, sizeof(date), "%02d/%02d/%04d", month, day, year);
    if (!validate_date(date)) {
        printf("Invalid date.\n");
        pause_screen();
        return;
    }

    // Step 6: Get start time
    valid = 0;
    do {
        clear_screen();
        printf("MAKE A RESERVATION\n");
        printf("-------------------\n\n");
        printf("Selected Room: %s\n", consultation_room);
        printf("Student Name: %s\n", student_name);
        printf("Student Number: %s\n", student_num);
        printf("Date: %s\n\n", date);
        
        printf("Enter start time (HH:MM AM/PM, e.g., 12:00PM, or 'cancel' to abort): ");
        
        if (scanf("%s", start_time) != 1){
            clear_input_buffer();
            printf("Error reading start time. Please try again.\n");
            pause_screen();
            continue;
        }
        clear_input_buffer();
        
        if (check_cancel_string(start_time)) {
            printf("Reservation cancelled.\n");
            pause_screen();
            return;
        }
        
        to_uppercase(start_time);

        if (!validate_time(start_time)){
            printf("Invalid time format. Please try again.\n");
            pause_screen();
            continue;
        }
        valid = 1;
    } while (!valid);

    // Step 7: Get end time
    valid = 0;
    do {
        clear_screen();
        printf("MAKE A RESERVATION\n");
        printf("-------------------\n\n");
        printf("Selected Room: %s\n", consultation_room);
        printf("Student Name: %s\n", student_name);
        printf("Student Number: %s\n", student_num);
        printf("Date: %s\n", date);
        printf("Start Time: %s\n\n", start_time);
        
        printf("Enter end time (HH:MM AM/PM, e.g., 01:00PM, or 'cancel' to abort): ");
        
        if (scanf("%s", end_time) != 1){
            clear_input_buffer();
            printf("Error reading end time. Please try again.\n");
            pause_screen();
            continue;
        }
        clear_input_buffer();
        
        if (check_cancel_string(end_time)) {
            printf("Reservation cancelled.\n");
            pause_screen();
            return;
        }
        
        to_uppercase(end_time);

        if (!validate_time(end_time)){
            printf("Invalid time format. Please try again.\n");
            pause_screen();
            continue;
        }

        if (!validate_time_range(start_time, end_time)){
            printf("End time must be after start time. Please try again.\n");
            pause_screen();
            continue;
        }
        valid = 1;
    } while (!valid);

    // Step 8: Check for conflicts
    if (check_time_conflict(date, start_time, end_time, consultation_room)) {
        clear_screen();
        printf("MAKE A RESERVATION\n");
        printf("-------------------\n\n");
        printf("Time slot is already reserved. Please try a different time.\n");
        pause_screen();
        return;
    }  

    // Generate reservation id
    generate_reservation_id(reservation_id, sizeof(reservation_id), date);
    
    // Confirm details
    clear_screen();
    printf("MAKE A RESERVATION\n");
    printf("-------------------\n\n");
    printf("Generated Reservation ID: %s\n\n", reservation_id);
    printf("Please confirm your reservation details:\n");
    printf("+----------------------+------------+-----------------+------------+------------+---------------------------+\n");
    printf("| %-20s | %-10s | %-15s | %-10s | %-10s | %-25s |\n",
        "Reservation ID", "Date", "Room", "Start Time", "End Time", "Student Name");
    printf("+----------------------+------------+-----------------+------------+------------+---------------------------+\n");
    printf("| %-20s | %-10s | %-15s | %-10s | %-10s | %-25s |\n",
        reservation_id, date, consultation_room, start_time, end_time, student_name);
    printf("+----------------------+------------+-----------------+------------+------------+---------------------------+\n");
    printf("\nConfirm reservation? (Y/N): ");
    
    char confirm;
    if (scanf(" %c", &confirm) != 1 || (confirm != 'Y' && confirm != 'y')) {
        clear_input_buffer();
        printf("Reservation cancelled by user.\n");
        pause_screen();
        return;
    }
    clear_input_buffer();
    
    // Create reservation
    int result = insert_reservation(student_name, student_num, date, start_time, end_time, reservation_id, consultation_room);
    if (result == 0) {
        printf("\nReservation created successfully!\n");
    } else {
        printf("\nFailed to create reservation.\n");
    }
    pause_screen();
}

void generate_reservation_id(char* out_id, size_t out_size, const char* reservation_date) {
    // Simple format: TODAY_DATE-RESERVATION_DATE-TIMESTAMP
    // Example: 101325-120125-123456 (MMDDYY-MMDDYY-HHMMSS)
    
    if (out_size < 20) {
        if (out_size > 0) out_id[0] = '\0';
        return;
    }
    
    // Get current time
    time_t now = time(NULL);
    struct tm* today = localtime(&now);
    
    // Today's date in MMDDYY format
    char today_str[7];
    strftime(today_str, sizeof(today_str), "%m%d%y", today);
    
    // Convert reservation date from MM/DD/YYYY to MMDDYY format
    char res_date_str[8]; // Increased size to [prevent truncation]
    if (strlen(reservation_date) == 10 && reservation_date[2] == '/' && reservation_date[5] == '/') {
        int month, day, year;
        if (sscanf(reservation_date, "%2d/%2d/%4d", &month, &day, &year) == 3)   {
            snprintf(res_date_str, sizeof(res_date_str), "%02d%02d%02d", 
                     month, day, year % 100);
        } else {
            strcpy(res_date_str, "000000");
        }
    } else {
        strcpy(res_date_str, "000000");
    }
    
    // timestamp for uniq (HHMMSS)
    char timestamp_str[7];
    strftime(timestamp_str, sizeof(timestamp_str), "%H%M%S", today);
    
    // ID FORMAT: TODAY_DATE-RESERVATION_DATE-TIMESTAMP
    snprintf(out_id, out_size, "%s-%s-%s", today_str, res_date_str, timestamp_str);
}

void cancel_reservation(){
    //cancel logic here
    int choice;
    char reservation_id[30];

    do {
    clear_screen();
    printf("CANCEL A RESERVATION\n");
    printf("--------------------\n");
    printf("Cancel by:\n");
    printf("1. Reservation ID\n");
    printf("2. Return to Main Menu\n");
    printf("Enter your choice: ");
    if (scanf("%d", &choice) != 1) {
        clear_input_buffer();
        printf("Invalid input.\n");
        pause_screen();
        continue; //ask again f invalid input
    }
    clear_input_buffer();

    switch (choice) {
    case 1: {
        int valid_reservation = 0;
        
        do {
            printf("Enter Reservation ID (to cancel operation, input 'cancel' to go back): ");
            if (scanf("%29s", reservation_id) != 1) {
                clear_input_buffer();
                pause_screen();
                continue;
            }
            clear_input_buffer();
            
            // Allow user to cancel operation
            // Convert to lowercase for comparison
            char temp[30];
            strcpy(temp, reservation_id);
            for(int i = 0; temp[i]; i++){
                temp[i] = tolower(temp[i]);
            }
            
            if (strcmp(temp, "cancel") == 0) {
                printf("Operation cancelled.\n");
                pause_screen();
                valid_reservation = 1; // Exit inner loop
                break;
            }
            
            // Check if reservation exists
            int exists = reservation_exists(reservation_id);
            
            if (exists == 0) {
                printf("\nNo reservation found with ID '%s'.\n", reservation_id);
                printf("Please check the ID and try again.\n\n");
                pause_screen();
                continue; // Ask again
            } else if (exists == -1) {
                printf("Database error occurred.\n");
                pause_screen();
                continue;
            }
            
            // Display
            printf("\nReservation found:\n");
            printf("| %-20s | %-10s | %-15s | %-10s | %-10s | %-25s |\n", 
                   "Reservation ID", "Date", "Room", "Start", "End", "Student Name");
            printf("|----------------------|------------|-----------------|------------|------------|---------------------------|\n");
            get_reservations_by_id(reservation_id);
            printf("|----------------------|------------|-----------------|------------|------------|---------------------------|\n");
            
            // Ask for confirmation
            printf("\nAre you sure you want to cancel this reservation? (Y/N): ");
            char confirm;
            scanf(" %c", &confirm);
            clear_input_buffer();
            
            if (confirm == 'Y' || confirm == 'y') {
                int result = delete_reservation(reservation_id);
                if (result == 0) {
                    printf("\nReservation '%s' successfully cancelled.\n", reservation_id);
                } else {
                    printf("\nFailed to cancel reservation.\n");
                }
            } else {
                printf("\nCancellation aborted.\n");
            }
            
            pause_screen();
            valid_reservation = 1; // Exit loop
            
        } while (!valid_reservation);
        
        break;
    }
    case 2:
        return;
    default:
        printf("Invalid choice. Please try again.\n");
        pause_screen();
    }
    } while (choice != 2);
}

void edit_reservation(){
    int choice;
    char reservation_id[30];

    do {
        clear_screen();
        printf("EDIT A RESERVATION\n");
        printf("--------------------\n");
        printf("Edit by:\n");
        printf("1. Reservation ID\n");
        printf("2. Return to Main Menu\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("Invalid input.\n");
            pause_screen();
            continue;
        }
        clear_input_buffer();

        switch(choice) {
            case 1: {
                int valid_reservation = 0;
                do {
                    clear_screen();
                    printf("EDIT A RESERVATION\n");
                    printf("--------------------\n\n");
                    printf("Enter Reservation ID to edit (or 'cancel' to go back): ");
                    
                    if (scanf("%29s", reservation_id) != 1) {
                        clear_input_buffer();
                        printf("Invalid input. Please try again.\n");
                        pause_screen();
                        continue;
                    }
                    clear_input_buffer();

                    // ALLOW USER TO CANCEL OPERATION
                    if (check_cancel_string(reservation_id)) {
                        printf("Operation cancelled.\n");
                        pause_screen();
                        valid_reservation = 1;
                        break;
                    }

                    // CHECK IF RESERVATION EXISTS
                    int exists = reservation_exists(reservation_id);
                    if (exists == 0){
                        printf("\nNo reservation found with ID '%s'.\n", reservation_id);
                        printf("Please check the ID and try again.\n\n");
                        pause_screen();
                        continue;
                    } else if (exists == -1){
                        pause_screen();
                        continue;
                    }

                    // DISPLAY
                    printf("\nReservation found:\n");
                    printf("| %-20s | %-10s | %-15s | %-10s | %-10s | %-25s |\n", 
                           "Reservation ID", "Date", "Room", "Start", "End", "Student Name");
                    printf("|----------------------|------------|-----------------|------------|------------|---------------------------|\n");
                    get_reservations_by_id(reservation_id);
                    printf("|----------------------|------------|-----------------|------------|------------|---------------------------|\n");
            
                    clear_input_buffer();

                    // GET NEW DETAILS AND VALIDATE THEM
                    char student_name[MAX_NAME_LENGTH];
                    char student_num[MAX_STUD_ID_LENGTH];
                    char date[MAX_DATE_LENGTH];
                    char start_time[MAX_TIME_LENGTH];
                    char end_time[MAX_TIME_LENGTH];
                    char consultation_room[MAX_ROOM_LENGTH];
                    char input_buffer[50];

                    // STUDENT NAME
                    int valid = 0;
                    do {
                        clear_screen();
                        printf("EDIT A RESERVATION\n");
                        printf("--------------------\n\n");
                        printf("Reservation ID: %s\n\n", reservation_id);
                        printf("Enter new student name (or 'cancel' to abort): ");
                        
                        if (fgets(student_name, sizeof(student_name), stdin) == NULL) {
                            printf("Error reading input. Please try again.\n");
                            pause_screen();
                            continue;
                        }
                        student_name[strcspn(student_name, "\n")] = 0;

                        if (check_cancel_string(student_name)) {
                            printf("Edit cancelled.\n");
                            pause_screen();
                            valid_reservation = 1;
                            valid = 1;
                            break;
                        }

                        if (strlen(student_name) == 0) {
                            printf("Name cannot be empty.\n");
                            pause_screen();
                            continue;
                        }
                        valid = 1;
                    } while (!valid);
                    
                    if (valid_reservation) break;

                    // STUDENT NUMBER
                    valid = 0;
                    do {
                        clear_screen();
                        printf("EDIT A RESERVATION\n");
                        printf("--------------------\n\n");
                        printf("Reservation ID: %s\n", reservation_id);
                        printf("Student Name: %s\n\n", student_name);
                        printf("Enter new student number (or 'cancel' to abort): ");
                        
                        if (scanf("%s", student_num) != 1) {
                            clear_input_buffer();
                            printf("Error reading input. Please try again.\n");
                            pause_screen();
                            continue;
                        }
                        clear_input_buffer();

                        if (check_cancel_string(student_num)) {
                            printf("Edit cancelled.\n");
                            pause_screen();
                            valid_reservation = 1;
                            valid = 1;
                            break;
                        }

                        if (strlen(student_num) != 10 || 
                            strspn(student_num, "0123456789") != 8 || 
                            student_num[8] != '-' || 
                            !isalnum((unsigned char)student_num[9])) {
                            printf("Invalid student number format.\n");
                            pause_screen();
                            continue;
                        }
                        valid = 1;
                    } while (!valid);
                    
                    if (valid_reservation) break;

                    // CONSULTATION ROOM
                    valid = 0;
                    do {
                        clear_screen();
                        printf("EDIT A RESERVATION\n");
                        printf("--------------------\n\n");
                        printf("Reservation ID: %s\n", reservation_id);
                        printf("Student Name: %s\n", student_name);
                        printf("Student Number: %s\n\n", student_num);
                        
                        printf("Available Consultation Rooms:\n");
                        for (int i = 0; i < NUM_ROOMS; i++) {
                            printf("%d. %s\n", i + 1, CONSULTATION_ROOMS[i]);
                        }
                        printf("Select consultation room (or 'cancel' to abort): ");

                        if (scanf("%s", input_buffer) != 1) {
                            clear_input_buffer();
                            printf("Invalid input. Please try again.\n");
                            pause_screen();
                            continue;
                        }
                        clear_input_buffer();

                        if (check_cancel_string(input_buffer)) {
                            printf("Edit cancelled.\n");
                            pause_screen();
                            valid_reservation = 1;
                            valid = 1;
                            break;
                        }

                        int room_choice = atoi(input_buffer);
                        if (room_choice < 1 || room_choice > NUM_ROOMS) {
                            printf("Invalid choice. Please try again.\n");
                            pause_screen();
                            continue;
                        }
                        
                        strcpy(consultation_room, CONSULTATION_ROOMS[room_choice - 1]);
                        valid = 1;
                    } while (!valid);
                    
                    if (valid_reservation) break;

                    // DATE - MONTH
                    int month, day, year, mday;
                    valid = 0;
                    do {
                        clear_screen();
                        printf("EDIT A RESERVATION\n");
                        printf("--------------------\n\n");
                        printf("Reservation ID: %s\n", reservation_id);
                        printf("Student Name: %s\n", student_name);
                        printf("Student Number: %s\n", student_num);
                        printf("Selected Room: %s\n\n", consultation_room);
                        
                        printf("Select new month:\n");
                        printf("1. January    2. February   3. March      4. April\n");
                        printf("5. May        6. June       7. July       8. August\n");
                        printf("9. September  10. October   11. November  12. December\n");
                        printf("Enter month (1-12, or 'cancel' to abort): ");

                        if (scanf("%s", input_buffer) != 1) {
                            clear_input_buffer();
                            printf("Invalid month. Please try again.\n");
                            pause_screen();
                            continue;
                        }
                        clear_input_buffer();

                        if (check_cancel_string(input_buffer)) {
                            printf("Edit cancelled.\n");
                            pause_screen();
                            valid_reservation = 1;
                            valid = 1;
                            break;
                        }

                        month = atoi(input_buffer);
                        if (month < 1 || month > 12) {
                            printf("Invalid month. Please try again.\n");
                            pause_screen();
                            continue;
                        }

                        switch (month) {
                            case 1: case 3: case 5: case 7: case 8: case 10: case 12: 
                                mday = 31; break;
                            case 4: case 6: case 9: case 11: 
                                mday = 30; break;
                            case 2: 
                                mday = 29; break;
                        }
                        valid = 1;
                    } while (!valid);
                    
                    if (valid_reservation) break;

                    // DAY
                    valid = 0;
                    do {
                        clear_screen();
                        printf("EDIT A RESERVATION\n");
                        printf("--------------------\n\n");
                        printf("Reservation ID: %s\n", reservation_id);
                        printf("Student Name: %s\n", student_name);
                        printf("Student Number: %s\n", student_num);
                        printf("Selected Room: %s\n", consultation_room);
                        printf("Month: %d\n\n", month);
                        
                        printf("Enter day (1-%d, or 'cancel' to abort): ", mday);
                        
                        if (scanf("%s", input_buffer) != 1) {
                            clear_input_buffer();
                            printf("Invalid input. Please try again.\n");
                            pause_screen();
                            continue;
                        }
                        clear_input_buffer();

                        if (check_cancel_string(input_buffer)) {
                            printf("Edit cancelled.\n");
                            pause_screen();
                            valid_reservation = 1;
                            valid = 1;
                            break;
                        }

                        day = atoi(input_buffer);
                        if(day < 1 || day > mday) {
                            printf("Invalid day for chosen month. Please try again.\n");
                            pause_screen();
                            continue;
                        }
                        valid = 1;
                    } while (!valid);
                    
                    if (valid_reservation) break;
                    
                    // AUTO SET YEAR
                    time_t now = time(NULL);
                    struct tm *t = localtime(&now);
                    year = t->tm_year + 1900;
                    printf("\nYear has been automatically set to %d.\n", year);
                    
                    snprintf(date, sizeof(date), "%02d/%02d/%04d", month, day, year);

                    // START TIME
                    valid = 0;
                    do {
                        clear_screen();
                        printf("EDIT A RESERVATION\n");
                        printf("--------------------\n\n");
                        printf("Reservation ID: %s\n", reservation_id);
                        printf("Student Name: %s\n", student_name);
                        printf("Student Number: %s\n", student_num);
                        printf("Selected Room: %s\n", consultation_room);
                        printf("Date: %s\n\n", date);
                        
                        printf("Enter new start time (HH:MM AM/PM, e.g., 12:00PM, or 'cancel' to abort): ");
                        
                        if (scanf("%s", start_time) != 1){
                            clear_input_buffer();
                            printf("Error reading start time. Please try again.\n");
                            pause_screen();
                            continue;
                        }
                        clear_input_buffer();

                        if (check_cancel_string(start_time)) {
                            printf("Edit cancelled.\n");
                            pause_screen();
                            valid_reservation = 1;
                            valid = 1;
                            break;
                        }

                        to_uppercase(start_time);

                        if (!validate_time(start_time)){
                            printf("Invalid time format. Please try again.\n");
                            pause_screen();
                            continue;
                        }
                        valid = 1;
                    } while (!valid);
                    
                    if (valid_reservation) break;
                    
                    // END TIME
                    valid = 0;
                    do {
                        clear_screen();
                        printf("EDIT A RESERVATION\n");
                        printf("--------------------\n\n");
                        printf("Reservation ID: %s\n", reservation_id);
                        printf("Student Name: %s\n", student_name);
                        printf("Student Number: %s\n", student_num);
                        printf("Selected Room: %s\n", consultation_room);
                        printf("Date: %s\n", date);
                        printf("Start Time: %s\n\n", start_time);
                        
                        printf("Enter new end time (HH:MM AM/PM, e.g., 01:00PM, or 'cancel' to abort): ");
                        
                        if (scanf("%s", end_time) != 1){
                            clear_input_buffer();
                            printf("Error reading end time. Please try again.\n");
                            pause_screen();
                            continue;
                        }
                        clear_input_buffer();

                        if (check_cancel_string(end_time)) {
                            printf("Edit cancelled.\n");
                            pause_screen();
                            valid_reservation = 1;
                            valid = 1;
                            break;
                        }

                        to_uppercase(end_time);

                        if (!validate_time(end_time)){
                            printf("Invalid time format. Please try again.\n");
                            pause_screen();
                            continue;
                        }

                        if (!validate_time_range(start_time, end_time)){
                            printf("End time must be after start time. Please try again.\n");
                            pause_screen();
                            continue;
                        }
                        valid = 1;
                    } while (!valid);
                    
                    if (valid_reservation) break;

                    // CONFIRM CHANGES
                    clear_screen();
                    printf("EDIT A RESERVATION\n");
                    printf("--------------------\n\n");
                    printf("New reservation details:\n");
                    printf("+----------------------+------------+-----------------+------------+------------+---------------------------+\n");
                    printf("| %-20s | %-10s | %-15s | %-10s | %-10s | %-25s |\n",
                        "Reservation ID", "Date", "Room", "Start Time", "End Time", "Student Name");
                    printf("+----------------------+------------+-----------------+------------+------------+---------------------------+\n");
                    printf("| %-20s | %-10s | %-15s | %-10s | %-10s | %-25s |\n",
                        reservation_id, date, consultation_room, start_time, end_time, student_name);
                    printf("+----------------------+------------+-----------------+------------+------------+---------------------------+\n");
                    
                    printf("\nConfirm changes? (Y/N): ");
                    char confirm;
                    scanf(" %c", &confirm);
                    clear_input_buffer();
                    
                    if (confirm == 'Y' || confirm == 'y') {
                        int result = update_reservation(reservation_id, student_name, student_num, 
                                                       date, start_time, end_time, consultation_room);
                        if (result == 0) {
                            printf("\nReservation '%s' successfully updated.\n", reservation_id);
                        } else {
                            printf("\nFailed to update reservation.\n");
                        }
                    } else {
                        printf("\nUpdate cancelled.\n");
                    }
                    
                    pause_screen();
                    valid_reservation = 1;
                    
                } while (!valid_reservation);
                break;
            }
            case 2:
                return;
            default:
                printf("Invalid choice. Please try again.\n");
                pause_screen();
        }
    } while (choice != 2);
}

void search_reservations(){
    int choice;
    char search_term[MAX_NAME_LENGTH];
    char search_id[MAX_RESERVATION_ID_LENGTH];

    do {
    clear_screen();
    printf("SEARCH RESERVATIONS\n");
    printf("-------------------\n");
    printf("Search by:\n");
    printf("1. Student Name\n");
    printf("2. Reservation ID\n");
    printf("3. Back to Main Menu\n\n");
    printf("Enter your choice (1-3): ");

    if (scanf("%d", &choice) != 1) {
        clear_input_buffer();
        printf("Invalid input.\n");
        pause_screen();
        continue;
    }
    clear_input_buffer();
        switch (choice) {
        case 1:
            printf("Enter student name (or 'cancel' to abort): "); // Clear input buffer
            if (fgets(search_term, sizeof(search_term), stdin) == NULL) {
                printf("Error reading name.\n");
                pause_screen();
                break;
            }
            
            search_term[strcspn(search_term, "\n")] = 0; // Remove newline

            if (check_cancel_string(search_term)) {
                printf("Reservation cancelled.\n");
                pause_screen();
                break;
            }
        
            if (strlen(search_term) == 0) {
                printf("Name cannot be empty. Please try again.\n");
                pause_screen();
                continue;
            }

            printf("\nSearch results for '%s':\n", search_term);
            printf("+----------------------+------------+-----------------+------------+------------+---------------------------+\n");
            printf("| %-20s | %-10s | %-15s | %-10s | %-10s | %-25s |\n",
                "Reservation ID", "Date", "Room", "Start Time", "End Time", "Student Name");
            printf("+----------------------+------------+-----------------+------------+------------+---------------------------+\n");
            int result = get_reservations_by_name(search_term);
                if (result != 0) {
                    printf("| %-112s |\n", "No reservations found.");
                }
            printf("+----------------------+------------+-----------------+------------+------------+---------------------------+\n");
            pause_screen();
            break;

        case 2: {
            int valid_id = 0;
            do {
                printf("Enter reservation ID (or 'cancel' to go back): ");
                    if (scanf("%29s", search_id) != 1) {
                        clear_input_buffer();
                        printf("Error reading reservation ID.\n");
                        pause_screen();
                        continue;
                    }
                clear_input_buffer();
                    
                    // Allow user to cancel
                char temp[MAX_RESERVATION_ID_LENGTH];
                strcpy(temp, search_id);
                for(int i = 0; temp[i]; i++){
                        temp[i] = tolower(temp[i]);
                    }
                    
                if (strcmp(temp, "cancel") == 0) {
                    printf("Search cancelled.\n");
                    pause_screen();
                    valid_id = 1;
                    break;
                }
                    
                // Check if reservation exists
                int exists = reservation_exists(search_id);
                    
                if (exists == 0) {
                    printf("\nNo reservation found with ID '%s'.\n", search_id);
                    printf("Please check the ID and try again.\n\n");
                    pause_screen();
                    continue;
                } else if (exists == -1) {
                    printf("Database error occurred.\n");
                    pause_screen();
                    continue;
                }

                printf("\nSearch results for ID '%s':\n", search_id);
                printf("+----------------------+------------+-----------------+------------+------------+---------------------------+\n");
                printf("| %-20s | %-10s | %-15s | %-10s | %-10s | %-25s |\n",
                    "Reservation ID", "Date", "Room", "Start Time", "End Time", "Student Name");
                printf("+----------------------+------------+-----------------+------------+------------+---------------------------+\n");
                get_reservations_by_id(search_id);
                printf("+----------------------+------------+-----------------+------------+------------+---------------------------+\n");
                    
                pause_screen();
                valid_id = 1;
                    
            } while (!valid_id);
                
            break;
        }
        case 3:
            return;

        default:
            printf("Invalid choice.\n");
            pause_screen();
            break;
        }
    } while(choice != 3);
}

void exit_program(){
    printf("Exiting the program...\n");
    sleep(3); //3s DELAY BEFORE EXITING
    sqlite3_close(db); //CLOSE DB
    exit(0); //EXIT PROGRAM
}

void clear_screen(){
    #ifdef _WIN32 // checks if the code is compiling on Windows
    system("cls"); // executes the Windows command to clear the screen
    #else
    system("clear"); // Unix equivalent
    #endif
}

void pause_screen(){
    printf("\nPress enter to continue...");
    getchar(); //clears newline from previous input
}
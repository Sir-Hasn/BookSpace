#include "headers/reservation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>


int validate_date(const char* date) {

    // Check if date is in the past
    time_t t = time(NULL); // Grabs current time and stores it in variable t
    struct tm *today = localtime(&t); // Converts t to localtime as structure broken down to year month day
    
    // Extract the current year, month, and day numbers from today
    int current_year = today->tm_year + 1900; // Counts years since 1900, e.g 1900 + 125 = 2025
    int current_month = today->tm_mon + 1; // Months start at 0 (= January), and +1 is added so 1 = January
    int current_day = today->tm_mday; // Gives the current day of the month (1-31)

    // Expected format: MM/DD/YYYY
    if (strlen(date) != 10) {
        return 0; // Invalid length
    }
    
    // Check format MM/DD/YYYY
    if (date[2] != '/' || date[5] != '/') {
        return 0; // Invalid separators
    }
    
    // Check if all other characters are digits
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue; // Skip separators
        if (date[i] < '0' || date[i] > '9') {
            return 0; // Non-digit character
        }
    }
    
    // Extract month, day, year
    int month = (date[0] - '0') * 10 + (date[1] - '0');
    int day = (date[3] - '0') * 10 + (date[4] - '0');
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + 
               (date[8] - '0') * 10 + (date[9] - '0');
    
    // Basic validation
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;
    if (year < current_year || year > current_year + 5) return 0; // Reasonable year range
    

    // Check days in month
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Leap year check
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
        days_in_month[1] = 29;
    }
    
    if (day > days_in_month[month - 1]) return 0; // Invalid day for month

    
    // Check if user input date is earlier than today
    if (year < current_year || // If year is smaller, it is in the past
       (year == current_year && month < current_month) || // If month is smaller, it is in the past
       (year == current_year && month == current_month && day < current_day)) { // If both year and month are smaller, it is in the past
       printf("Error: A past date is invalid.\n"); // Invalid date because any of the three conditions are the past
        return 0; // Mark the date as invalid
    }

    return 1; // Valid date
}

int validate_date_for_viewing(const char* date){

    // Check if date is in the past
    time_t t = time(NULL); // Grabs current time and stores it in variable t
    struct tm *today = localtime(&t); // Converts t to localtime as structure broken down to year month day
    
    // Extract the current year, month, and day numbers from today
    int current_year = today->tm_year + 1900; // Counts years since 1900, e.g 1900 + 125 = 2025
    int current_month = today->tm_mon + 1; // Months start at 0 (= January), and +1 is added so 1 = January
    int current_day = today->tm_mday; // Gives the current day of the month (1-31)


      // Expected format: MM/DD/YYYY
    if (strlen(date) != 10) {
        return 0; // Invalid length
    }
    
    // Check format MM/DD/YYYY
    if (date[2] != '/' || date[5] != '/') {
        return 0; // Invalid separators
    }
    
    // Check if all other characters are digits
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue; // Skip separators
        if (date[i] < '0' || date[i] > '9') {
            return 0; // Non-digit character
        }
    }
    
    // Extract month, day, year
    int month = (date[0] - '0') * 10 + (date[1] - '0');
    int day = (date[3] - '0') * 10 + (date[4] - '0');
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + 
               (date[8] - '0') * 10 + (date[9] - '0');
    
    // Basic validation
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;
    if (year < current_year || year > current_year + 5) return 0; // Reasonable year range
    

    // Check days in month
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Leap year check
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
        days_in_month[1] = 29;
    }
    
    if (day > days_in_month[month - 1]) return 0; // Invalid day for month

    return 1; // Valid date
}

int validate_time(const char* time) {
    // Expected format: HH:MM AM/PM
    if (strlen(time) < 6 || strlen(time) > 8) {
        return 0; // Invalid length
    }
    
    // Check for AM/PM
    char* ampm = strstr(time, "AM");
    if (!ampm) ampm = strstr(time, "PM");
    if (!ampm) return 0; // No AM/PM found
    
    // Check if AM/PM is at the end
    if (strcmp(ampm, "AM") != 0 && strcmp(ampm, "PM") != 0) {
        return 0; // AM/PM not at the end
    }
    
    // Extract time part (before AM/PM)
    int time_len = ampm - time;
    if (time_len < 4 || time_len > 5) return 0; // Invalid time length
    
    // Check for colon
    int colon_pos = -1;
    for (int i = 0; i < time_len; i++) {
        if (time[i] == ':') {
            colon_pos = i;
            break;
        }
    }
    
    if (colon_pos == -1 || colon_pos == 0 || colon_pos == time_len - 1) {
        return 0; // No colon or colon in wrong position
    }
    
    // Check hour part
    for (int i = 0; i < colon_pos; i++) {
        if (time[i] < '0' || time[i] > '9') return 0;
    }
    
    // Check minute part
    for (int i = colon_pos + 1; i < time_len; i++) {
        if (time[i] < '0' || time[i] > '9') return 0;
    }
    
    // Extract and validate hour and minute
    int hour = 0, minute = 0;
    sscanf(time, "%d:%d", &hour, &minute);
    
    if (hour < 1 || hour > 12) return 0;
    if (minute < 0 || minute > 59) return 0;
    
    return 1; // Valid time
}

int validate_time_range(const char* start_time, const char* end_time) {
    char start_24[MAX_TIME_LENGTH], end_24[MAX_TIME_LENGTH];
    
    format_time_24hour((char*)start_time, start_24);
    format_time_24hour((char*)end_time, end_24);
    
    return compare_times(start_24, end_24) < 0;
}

int is_time_conflict(const char* start1, const char* end1, const char* start2, const char* end2) {
    // Check if time ranges overlap
    return (compare_times(start1, end2) < 0 && compare_times(end1, start2) > 0);
}

void format_time_12hour(char* time_24, char* time_12) {
    // Convert 24-hour format (HH:MM) to 12-hour format (HH:MM AM/PM)
     int hour, minute;
    sscanf(time_24, "%d:%d", &hour, &minute);
    
    int hour_12 = hour;
    char* period = "AM";
    
    if (hour == 0) {
        hour_12 = 12;
    } else if (hour == 12) {
        period = "PM";
    } else if (hour > 12) {
        hour_12 = hour - 12;
        period = "PM";
    }
    
    sprintf(time_12, "%d:%02d %s", hour_12, minute, period);
}

void format_time_24hour(char* time_12, char* time_24) {
    // Convert 12-hour format (HH:MM AM/PM) to 24-hour format (HH:MM)

    if (!validate_time(time_12)) {
    time_24[0] = '\0'; // Invalid input
    return;
}


    int hour, minute;
    char period[3];
    sscanf(time_12, "%d:%d %2s", &hour, &minute, period);
    
    if (strcmp(period, "PM") == 0 && hour != 12) {
        hour += 12;
    } else if (strcmp(period, "AM") == 0 && hour == 12) {
        hour = 0;
    }
    
    sprintf(time_24, "%02d:%02d", hour, minute);
}

int compare_times(const char* time1, const char* time2) {
    // Compare two times in HH:MM format
    int hour1, minute1, hour2, minute2;
    sscanf(time1, "%d:%d", &hour1, &minute1);
    sscanf(time2, "%d:%d", &hour2, &minute2);
    
    if (hour1 != hour2) {
        return hour1 - hour2;
    }
    return minute1 - minute2;
}

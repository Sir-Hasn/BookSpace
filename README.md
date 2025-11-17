# Library Reservation System

A console-based reservation system designed to help librarians efficiently manage library reservation bookings.

## Features

- **View Daily Schedule**: Display reservations for specific dates
- **Make Reservations**: Create new room bookings with conflict detection and unique reservation id generation
- **Cancel Reservations**: Remove existing bookings by reservation ID
- **Edit Reservation**: Update/Edit existing booking by reservation ID
- **Search Reservations**: Find bookings by student name or reservation ID
- **Data Persistence**: SQLite database for reliable data storage
- **OFFLINE**

## Requirements

- C Compiler (GCC recommended)
- SQLite3 development libraries
- Windows/Linux/macOS

## Installation

1. Clone or download this repository
2. Ensure SQLite3 is installed on your system
3. Compile the program using the provided Makefile

## Compilation


## Usage

1. Run the executable: `./LibraryReservation`
2. Follow the menu prompts to manage reservations
3. The database file will be created automatically in the `data/` directory

## Project Structure

```
library-reservation-system/
├── src/
│   ├── MainProgram.c             # Main program file
│   ├── db.c                      # Database operations
│   ├── reservations.c            # Reservation management
│   ├── utilities.c               # Utility functions
│   └── headers/
│       ├── main.h               # Main header file
│       ├── database.h           # Database function declarations
│       └── reservations.h       # Reservation structure definitions
├── data/
│   └── library_reservations.db  # SQLite database file (auto-generated)
└── README.md                    # This file
```

## Database Schema

The system uses a single table `reservations` with the following structure:

- `id`: Primary key (auto-increment)
- `student_num`: Student's ID number (TEXT)
- `student_name`: Student's name (TEXT)
- `date`: Reservation date in MM/DD/YYYY format (TEXT)
- `start_time`: Start time in 24-hour format (TEXT)
- `end_time`: End time in 24-hour format (TEXT)
- `created_at`: Timestamp when reservation was created (DATETIME)

## Input Formats

- **Date**: MM/DD/YYYY (e.g., 12/15/2024)
- **Time**: HH:MM AM/PM (e.g., 09:00 AM, 02:30 PM)

## Error Handling

The system includes comprehensive error handling for:
- Invalid date/time formats
- Time conflict detection
- Database connection issues
- Input validation

## Development

This project follows the case study framework requirements and includes:
- Modular C programming structure
- SQLite3 database integration
- Input validation and error handling
- Menu-driven console interface
- Comprehensive documentation

## License

This project is created for educational purposes as part of a case study framework.

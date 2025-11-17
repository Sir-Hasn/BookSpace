# Library Consultation Room Reservation System

A console-based reservation management system built in **Pure C** for managing library consultation room bookings. This system helps librarians efficiently handle room reservations with conflict detection, validation, and persistent data storage using SQLite.

## Table of Contents

- [Features](#features)
- [System Requirements](#system-requirements)
- [Installation](#installation)
- [Compilation](#compilation)
- [Usage](#usage)
- [System Architecture](#system-architecture)
- [Database Schema](#database-schema)
- [Input Formats](#input-formats)
- [Project Structure](#project-structure)
- [Technical Details](#technical-details)
- [Development](#development)

## Features

### Core Functionality

- **View Daily Schedule**: Display all reservations for a specific date in a formatted table
- **Make Reservations**: Create new room bookings with:
  - Multiple consultation room selection (Room A, B, C, D)
  - Student information validation
  - Date and time validation
  - Automatic conflict detection (prevents double-booking)
  - Unique reservation ID generation
  - Confirmation before saving
- **Cancel Reservations**: Remove existing bookings by reservation ID with confirmation
- **Edit Reservations**: Update existing reservation details (name, student number, room, date, time)
- **Search Reservations**: Find bookings by:
  - Student name (partial match supported)
  - Reservation ID (exact match)
- **Data Persistence**: SQLite database for reliable, offline data storage
- **Input Validation**: Comprehensive validation for dates, times, student numbers, and names
- **Time Conflict Detection**: Prevents overlapping reservations in the same room
- **User-Friendly Interface**: Menu-driven console interface with clear prompts

### Validation Features

- **Date Validation**: 
  - Format: MM/DD/YYYY
  - Prevents past dates
  - Validates month/day ranges
  - Leap year support
- **Time Validation**:
  - Format: HH:MM AM/PM (e.g., 09:30 AM, 02:45 PM)
  - Library hours: 8:00 AM to 8:00 PM
  - Ensures end time is after start time
- **Student Number Validation**: Format: 8 digits + hyphen + 1 letter (e.g., 12345678-A)
- **Student Name Validation**: Alphabetic characters and spaces only

## System Requirements

- **Operating System**: Windows, Linux, or macOS
- **Compiler**: GCC (GNU Compiler Collection) or compatible C compiler
- **SQLite3**: Included in the project (sqlite3.c)
- **C Standard**: C11 or later

## Installation

1. **Clone or download this repository**
   ```bash
   git clone <repository-url>
   cd "Library Reservation"
   ```

2. **Ensure you have a C compiler installed**
   - **Windows**: Install MSYS2 with MinGW-w64 (recommended)
   - **Linux**: Install via package manager: `sudo apt-get install gcc` (Ubuntu/Debian)
   - **macOS**: Install Xcode Command Line Tools: `xcode-select --install`

3. **Verify SQLite3 is available**
   - The project includes `sqlite3.c` and `sqlite3.h` in the `src/` directory
   - No additional SQLite installation required

## Compilation

### Using GCC (Command Line)

#### Windows (MSYS2 UCRT64)
```bash
cd "/c/Users/Shan Ricz/Desktop/Library Reservation"
gcc -std=c11 -Wall -Wextra -I src/headers -I src \
    src/MainProgram.c src/db.c src/reservation.c src/utilities.c src/sqlite3.c \
    -o LibraryReservation.exe
```

#### Linux/macOS
```bash
cd "Library Reservation"
gcc -std=c11 -Wall -Wextra -I src/headers -I src \
    src/MainProgram.c src/db.c src/reservation.c src/utilities.c src/sqlite3.c \
    -o LibraryReservation
```

### Using VS Code

1. Open the project folder in VS Code
2. Press `Ctrl+Shift+B` (or `Cmd+Shift+B` on macOS) to build
3. The build task is configured in `.vscode/tasks.json`

### Build Options

- **`-std=c11`**: Use C11 standard
- **`-Wall -Wextra`**: Enable comprehensive warnings
- **`-I src/headers -I src`**: Include paths for headers and SQLite
- **`-o LibraryReservation.exe`**: Output executable name

## Usage

### Running the Program

#### Windows
```powershell
.\LibraryReservation.exe
```

#### Linux/macOS
```bash
./LibraryReservation
```

### Main Menu Options

1. **View Daily Schedule**
   - Enter a date in MM/DD/YYYY format
   - View all reservations for that date in a formatted table
   - Type 'cancel' to abort

2. **Make a Reservation**
   - Select consultation room (Room A, B, C, or D)
   - Enter student name (alphabetic characters and spaces only)
   - Enter student number (format: 12345678-A)
   - Select month and day (year auto-set to current year)
   - Enter start time (HH:MM AM/PM format)
   - Enter end time (must be after start time)
   - Confirm reservation details
   - System generates unique reservation ID automatically

3. **Cancel a Reservation**
   - Enter reservation ID
   - View reservation details
   - Confirm cancellation

4. **Edit Reservation**
   - Enter reservation ID
   - Update any field (name, student number, room, date, time)
   - System validates all inputs and checks for conflicts
   - Confirm changes

5. **Search Reservations**
   - Search by student name (partial match)
   - Search by reservation ID (exact match)
   - View results in formatted table

6. **Exit**
   - Closes database connection and exits program

### Canceling Operations

At any input prompt, type **'cancel'** (case-insensitive) to abort the current operation and return to the main menu.

## System Architecture

### Modular Design

The system follows a modular architecture with clear separation of concerns:

```
┌─────────────────────────────────────────┐
│         MainProgram.c                    │
│  - User interface & menu navigation     │
│  - Input handling                       │
│  - Program flow control                 │
└──────────────┬──────────────────────────┘
               │
       ┌───────┴────────┐
       │                 │
┌──────▼──────┐  ┌──────▼────────┐
│   db.c      │  │ reservation.c   │
│             │  │                 │
│ - Database  │  │ - Validation    │
│   operations│  │ - Time format   │
│ - SQLite    │  │ - Date/time     │
│   queries   │  │   logic         │
└─────────────┘  └─────────────────┘
       │
┌──────▼──────┐
│ utilities.c │
│             │
│ - Helper    │
│   functions │
│ - String    │
│   utilities │
└─────────────┘
```

### Component Responsibilities

- **MainProgram.c**: User interface, menu system, input collection, program orchestration
- **db.c**: Database operations (CRUD), SQLite integration, conflict checking
- **reservation.c**: Input validation (dates, times, formats), time conversion (12/24-hour)
- **utilities.c**: Helper functions (buffer clearing, string comparison, input sanitization)

## Database Schema

### Table: `reservations`

| Column | Type | Constraints | Description |
|--------|------|-------------|-------------|
| `id` | INTEGER | PRIMARY KEY, AUTOINCREMENT | Internal database ID |
| `student_name` | TEXT | NOT NULL | Student's full name |
| `student_num` | TEXT | NOT NULL | Student number (format: 12345678-A) |
| `date` | TEXT | NOT NULL | Reservation date (MM/DD/YYYY) |
| `start_time` | TEXT | NOT NULL | Start time (24-hour format: HH:MM) |
| `end_time` | TEXT | NOT NULL | End time (24-hour format: HH:MM) |
| `reservation_id` | TEXT | NOT NULL, UNIQUE | Unique reservation identifier |
| `consultation_room` | TEXT | NOT NULL | Room name (Room A, B, C, or D) |
| `created_at` | DATETIME | DEFAULT CURRENT_TIMESTAMP | Timestamp of creation |

### Constraints

- **Unique Constraint**: `(date, start_time, end_time, consultation_room)` - Prevents double-booking
- **Unique Constraint**: `reservation_id` - Ensures unique reservation identifiers

### Database Location

- **Path**: `data/library_reservations.db`
- **Auto-created**: Database and tables are created automatically on first run
- **Format**: SQLite3 database file

## Input Formats

### Date Format
- **Format**: `MM/DD/YYYY`
- **Example**: `12/25/2024`
- **Validation**:
  - Must be 10 characters
  - Month: 01-12
  - Day: Valid for the month (accounts for leap years)
  - Year: Current year to current year + 5
  - Cannot be a past date

### Time Format
- **Format**: `HH:MM AM/PM` or `H:MM AM/PM`
- **Examples**: 
  - `09:30 AM`
  - `2:45 PM`
  - `12:00 PM`
- **Validation**:
  - Must include AM or PM
  - Hour: 1-12
  - Minute: 00-59
  - Library hours: 8:00 AM to 8:00 PM
  - End time must be after start time

### Student Number Format
- **Format**: `DDDDDDDD-L` (8 digits, hyphen, 1 letter)
- **Example**: `12345678-A`
- **Validation**:
  - Exactly 10 characters
  - First 8 characters must be digits
  - 9th character must be hyphen
  - 10th character must be a letter

### Student Name Format
- **Format**: Alphabetic characters and spaces only
- **Example**: `John Doe`, `Maria Santos`
- **Validation**:
  - Cannot be empty
  - Only letters and spaces allowed
  - Case-insensitive

### Reservation ID Format
- **Format**: `TODAY_DATE-RESERVATION_DATE-TIMESTAMP`
- **Example**: `100225-120125-143052`
- **Generation**: Automatic (no user input required)
- **Components**:
  - Today's date: MMDDYY
  - Reservation date: MMDDYY
  - Timestamp: HHMMSS

## Project Structure

```
Library Reservation/
├── src/
│   ├── MainProgram.c          # Main program and user interface
│   ├── db.c                   # Database operations (SQLite)
│   ├── reservation.c          # Validation and time logic
│   ├── utilities.c           # Helper functions
│   ├── sqlite3.c              # SQLite3 library (embedded)
│   ├── sqlite3.h              # SQLite3 header
│   ├── sqlite3.dll            # SQLite3 DLL (Windows)
│   └── headers/
│       ├── main.h             # Main program declarations
│       ├── database.h         # Database function declarations
│       └── reservation.h     # Reservation structures and constants
├── data/
│   └── library_reservations.db  # SQLite database (auto-generated)
├── .vscode/
│   └── tasks.json             # VS Code build configuration
├── README.md                   # This file
└── Structure.txt              # Project structure notes
```

### File Descriptions

- **MainProgram.c**: Contains main menu, user interaction, and program flow
- **db.c**: All database operations including CRUD, conflict checking, and SQLite callbacks
- **reservation.c**: Date/time validation, format conversion (12/24-hour), time comparison
- **utilities.c**: Input buffer management, string utilities, cancel detection
- **headers/**: Header files with function declarations, constants, and structures

## Technical Details

### Time Handling

- **Storage Format**: 24-hour format (HH:MM) in database
- **Display Format**: 12-hour format (HH:MM AM/PM) for user interface
- **Conversion**: Automatic conversion between formats using `format_time_24hour()` and `format_time_12hour()`

### Conflict Detection

The system checks for time conflicts using SQL queries that detect overlapping time ranges:

```sql
SELECT COUNT(*) FROM reservations 
WHERE date = ? AND consultation_room = ? AND (
    (start_time < ? AND end_time > ?) OR
    (start_time >= ? AND start_time < ?) OR
    (end_time > ? AND end_time <= ?)
)
```

### Input Buffer Management

The system uses `clear_input_buffer()` to handle newline characters left in the input stream after `scanf()` calls, preventing input issues.

### Error Handling

- **Database Errors**: SQLite error messages displayed to user
- **Validation Errors**: Clear error messages for invalid inputs
- **Input Errors**: Graceful handling of invalid input with retry prompts

### Platform Compatibility

- **Windows**: Uses `system("cls")` for screen clearing
- **Unix/Linux/macOS**: Uses `system("clear")` for screen clearing
- **Compilation**: Platform-specific code handled via preprocessor directives

## Development

### Code Style

- **Language**: Pure C (C11 standard)
- **Naming**: snake_case for functions and variables
- **Modularity**: Clear separation of concerns across files
- **Comments**: Inline comments explaining complex logic

### Key Design Decisions

1. **SQLite Embedding**: SQLite3 is compiled directly into the executable for portability
2. **Console Interface**: Text-based interface for simplicity and cross-platform compatibility
3. **Input Validation**: Comprehensive validation at every input point
4. **Conflict Prevention**: Proactive conflict checking before database insertion
5. **Unique IDs**: Automatic generation ensures no duplicate reservation IDs

### Future Enhancements (Potential)

- Email notifications for reservations
- Export functionality (CSV, PDF)
- Multi-user support with authentication
- Web interface
- Calendar view
- Recurring reservations
- Room capacity management
- Waitlist functionality

### Building from Source

1. Ensure all source files are in the `src/` directory
2. Ensure header files are in `src/headers/`
3. Compile using the commands in the [Compilation](#compilation) section
4. Run the executable

### Troubleshooting

**Issue**: Database file not created
- **Solution**: Ensure the `data/` directory exists or the program has write permissions

**Issue**: Compilation errors
- **Solution**: Verify all include paths are correct (`-I src/headers -I src`)
- **Solution**: Ensure SQLite3 files (`sqlite3.c`, `sqlite3.h`) are present

**Issue**: Program crashes on input
- **Solution**: Ensure input buffer is cleared after `scanf()` calls (handled automatically)

## License

This project is created for educational purposes as part of a case study framework.

---

**Note**: This system is designed for offline use. All data is stored locally in the SQLite database file. No internet connection is required for operation.


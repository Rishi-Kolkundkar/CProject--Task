# Command-Line Task Manager in C

### 1. Project Description

This project is a comprehensive command-line task management application built entirely in C. It provides a full suite of features for user profile management, task organization, and progress tracking, all while prioritizing data security and a clean user experience.

### 2. Explanation of Project Functionalities

-   **Profile and Login:** Users can create a new profile or log in to an existing one with a secure, hashed password. Logout and profile switching are also supported.
-   **Password Hashing:** Implements SHA-256 hashing to avoid storing plain text passwords, enhancing security.
-   **Task Management:** After logging in, a user can:
    -   Create multiple tasks at once.
    -   View tasks with daily, weekly, or monthly views.
    -   Update all fields of an existing task.
    -   Set and automatically update a task's status (Pending, Overdue, Completed).
    -   Mark a task as complete.
    -   Delete tasks.
    -   Rearrange the priority of tasks.
-   **Reminders:** Automatically displays reminders for overdue tasks and tasks due today upon login.
-   **Reward System:** A leaderboard and reward system encourages task completion by awarding points based on timeliness and priority.

### 3. Detailed Breakdown of Functionalities

Our design philosophy was to build a robust and persistent application from the ground up. Here’s a breakdown of how the core components are implemented.

**I. Handling Users (Login & Profiles)**

For user management, we relied on a simple text file (`users.txt`) to act as our database.

-   **Creating a Profile:** When a user signs up, the program checks if the username is taken. If it's free, it takes their password, hashes it, and appends the new user details (username, hash, and starting reward) to the end of `users.txt`.
-   **Login:** We don't store plain text passwords. When a user tries to log in, we hash the entered password and scan `users.txt` line-by-line. If both the username and the generated hash match, they can access post-login functions.
-   **Logout:** This is simple flow control. We break the loop of the user menu, which sends the user back to the main landing screen.

**II. Security (Password Hashing)**

The core of our security is the SHA-256 algorithm. The moment a password is typed, it gets converted into a 64-character hexadecimal string. This hash is what actually gets stored and compared, so even if someone opens the `users.txt` file, they can't see the real passwords.

**III. Task Management**

For tasks, we used binary files (named `[username]_task.txt`) because they are faster to read and write for C structs than text files. The general pattern for almost every feature is **load everything -> update in memory -> save everything**.

-   **Creating Tasks:** We read the existing tasks into a dynamic array and use `realloc` to make space for new ones. Once the user fills in the details, we overwrite the entire binary file with the updated list.
-   **Viewing Tasks:** We load the whole task list into memory first. Then, depending on whether the user wants to see "Today," "Week," or "Month," we print using an `if` condition based on the current system date.
-   **Updating & Completing Tasks:** When updating, we let the user modify fields directly in the memory array. When completing, we change the status in memory and calculate points. Once done, the entire array is dumped back to the file.
-   **Deleting Tasks:** To delete, we find the task in the array, shift every subsequent task one position to the left, decrease the task count, and save the smaller array back to the file.

**IV. Rewards & Leaderboard**

-   **Earning points:** When a user finishes a task, we update their permanent record. We load all users from `users.txt` into an array, find the specific user, add their points, and then rewrite the entire `users.txt` file to save the new score.
-   **Leaderboard:** To show the rankings, we load the user list and run a simple Bubble Sort algorithm to order them by reward points (highest to lowest) before printing.

### 4. Member-wise Contributions

-   **Patel Karma Rohit:** Create User, Switch User, Profile (with reward system)
-   **Rishi K:** Login, Hashing, Create Task, Delete Task, Complete Task, Re-arrange
-   **Samarth Paavan Srinidhi:** View Tasks, Update Tasks, Reminders, Task Status

### 5. Function Explanations

Here is a breakdown of the functions within each source file.

#### `main.c`

-   `show_user_menu(const char* username)`
    Shows the task menu after a user logs in. It takes the username as input, calls task-related functions based on user choice, and runs until the user logs out.
-   `main()`
    The entry point of the program. It displays the main menu, handles user creation, login, leaderboard display, and application exit. It calls `show_user_menu()` after a successful login.

#### `user.c`

-   `usernameExists(const char* name)`
    Checks if a given username already exists in `users.txt`. Returns 1 if found, otherwise 0.
-   `hash_password(const char* password, char hex_output[65])`
    Hashes a plaintext password using SHA-256 and outputs a 64-character hex string.
-   `addUser()`
    Creates a new user, ensures the username is unique, hashes the password, and saves the record to `users.txt`.
-   `loadUsers(User **list, int *count)`
    Loads all users from the file into a dynamically allocated array of `User` structs.
-   `login(char* username_out)`
    Validates username and password against the database. Returns different codes to `main()` to signal success, failure, new user registration, or exit.
-   `get_hidden_password(char* buffer, int size)`
    Securely reads password input from the terminal without displaying the characters on screen.
-   `showProfile(User u)`
    Displays the details (name, DOB, rewards) of a given `User` structure.
-   `showCurrentUser(User* u)`
    Prints the profile of the currently logged-in user.
-   `showLeaderboard()`
    Loads all users, sorts them by highest reward points, and prints a ranked list.
-   `update_reward_score(const char* username, int points_to_add)`
    Adds reward points to a specific user and rewrites the entire `users.txt` file to save the change.

#### `tasks.c`

-   `is_valid_date(const char* date)`
    Validates a date string in `YYYY-MM-DD` format. Returns 1 if valid, 0 otherwise.
-   `fnamegen(const char* username, char* fname_out, size_t size)`
    Generates the unique filename (`[username]_task.txt`) where a user's tasks are stored.
-   `clear_input_buffer()`
    Flushes leftover characters from `stdin` to prevent input issues when mixing `scanf` and `fgets`.
-   `current_date(char* day, char* month, char* year)`
    Gets the system's current date and populates the provided character buffers.
-   `dayOfWeek(int day, int month, int year)`
    Calculates the day of the week (0-6) using Zeller's Congruence for calendar generation.
-   `day_name_convert(int day, int month, int year)`
    Returns a string ("Monday", etc.) for a specific date, used for labeling in views.
-   `LeapYear(int year)` & `days_in_month(int month, int year)`
    Utility functions to check for leap years and get the number of days in a month.
-   `create_new_tasks(const char* username)`
    Loads existing tasks, prompts the user to add new tasks, and saves the updated list back to the user's binary file.
-   `rearrange_tasks(const char* username)`
    Allows a user to manually change a task's priority by moving it to a new index in the list.
-   `set_status(Task* task)`
    Checks a task's due date against today's date and updates its status to `PENDING` or `OVERDUE`.
-   `view_tasks(const char* username)`
    Displays tasks in daily, weekly, or monthly formats with color-coded priorities and statuses.
-   `show_reminders(const char* username)`
    Displays overdue tasks and tasks due today immediately after a user logs in.
-   `update_task(const char* username)`
    Provides an interactive menu for a user to modify any field of a chosen task.
-   `delete_task(const char* username)`
    Deletes a task by its ID, shifts subsequent tasks in the array, and saves the updated list.
-   `complete_task(const char* username)`
    Marks a task as `COMPLETED`, calculates reward points based on timeliness and priority, and saves the change.


### How to Compile and Run

#### Recommended Setup (for Linux)

This application is best experienced on a Linux-based operating system, as it makes extensive use of ANSI color codes and special characters (like 🔔, 🚨, ⏳) for a richer user interface.

For the best visual experience and to ensure all emoji characters render correctly in your terminal, it is recommended to install the Noto Color Emoji font. You can do this on Debian/Ubuntu-based systems with the following command:

```bash
sudo apt-get install fonts-noto-color-emoji
```

#### Compilation and Execution

This project has no external dependencies other than a standard C compiler (like `gcc`).

1.  **Navigate** to the project directory in your terminal.
2.  **Compile** all the necessary source files with the following command:
    ```bash
    gcc -o task_manager main.c user.c tasks.c sha256.c
    ```
3.  **Run** the compiled application:
    ```bash
    ./task_manager
    ```
    (On Windows, you can run `task_manager.exe`, but be aware that some special characters and colors may not display correctly without a modern terminal like Windows Terminal.)

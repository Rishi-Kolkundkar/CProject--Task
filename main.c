#include <stdio.h>
#include "common.h" // Our shared header with all function prototypes

// This menu is shown after a user successfully logs in.
void show_user_menu(const char* username) {
    int choice;
    while(1) {
        printf("\n--- Task Menu for %s ---\n", username);
        printf("1. Create New Tasks\n");
        printf("2. View Tasks\n");
        printf("3. Update a Task\n");
        printf("4. Complete a Task\n");
        printf("5. Delete a Task\n");
        printf("6. Rearrange Task Priorities\n");
        printf("7. Show Reminders\n");
        printf("8. Logout\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clear_input_buffer(); // Clear the bad input
            continue;
        }
        clear_input_buffer(); // Clear the newline after the number

        switch(choice) {
            case 1: create_new_tasks(username); break;
            case 2: view_tasks(username); break;
            case 3: update_task(username); break;
            case 4: {
                int points = complete_task(username);
                if (points > 0) {
                    update_reward_score(username, points);
                }
                break;
            }
            case 5: delete_task(username); break;
            case 6: rearrange_tasks(username); break;
            case 7: show_reminders(username); break;
            case 8:
                printf("Logging out...\n");
                return; // Return to the main menu
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }
}

// The main entry point for the entire application.
int main() {
    char current_user[32] = {0}; // Holds the currently logged-in user's name

    printf("\n========== Welcome to the Task Manager ==========\n");

    while (1) {
        int choice;
        printf("\n========== Main Menu ==========\n");
        printf("1. Create User\n");
        printf("2. Login\n");
        printf("3. Show Leaderboard\n");
        printf("4. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                addUser();
                break;
            case 2:
                if (login(current_user)) { // login() returns 1 on success
                    printf("\n--------------------------------------\n");
                    show_reminders(current_user); // Show reminders upon login
                    printf("--------------------------------------\n");
                    show_user_menu(current_user); // Enter the user-specific task menu
                }
                break;
            case 3:
                showLeaderboard();
                break;
            case 4:
                printf("Goodbye!\n");
                return 0; // Exit the program
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    return 0;
}
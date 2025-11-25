#include <stdio.h>
#include "common.h"

// This menu is shown after a user successfully logs in.
void show_user_menu(const char* username) {
    int choice;
    while(1) {
        printf(CYAN BOLD "\n--- Task Menu for %s ---\n" RESET, username);
        printf(GREEN "1. Create New Tasks\n" RESET);
        printf(GREEN "2. View Tasks\n" RESET);
        printf(YELLOW "3. Update a Task\n" RESET);
        printf(YELLOW "4. Complete a Task\n" RESET);
        printf(RED "5. Delete a Task\n" RESET);
        printf(CYAN "6. Rearrange Task Priorities\n" RESET);
        printf(MAGENTA "7. Show Reminders\n" RESET);
        printf(WHITE "8. Logout\n" RESET);
        printf(BOLD "\nEnter your choice: " RESET);

        if (scanf("%d", &choice) != 1) {
            printf(RED "Invalid input. Please enter a number.\n" RESET);
            clear_input_buffer(); 
            continue;
        }
        clear_input_buffer();

        switch(choice) {
            case 1: create_new_tasks(username); break;
            case 2: view_tasks(username); break;
            case 3: update_task(username); break;
            case 4: {
                int points = complete_task(username);
                if (points > 0) {
                    printf(YELLOW "Updating your score...\n" RESET);
                    update_reward_score(username, points);
                }
                break;
            }
            case 5: delete_task(username); break;
            case 6: rearrange_tasks(username); break;
            case 7: show_reminders(username); break;
            case 8:
                printf(YELLOW "Logging out...\n" RESET);
                return;
            default:
                printf(RED "Invalid choice. Please try again.\n" RESET);
                break;
        }
    }
}

// This is the main entry point for the entire CLI Project.
int main() {
    char current_user[32] = {0}; 

    printf(BLUE BOLD "\n\n========== Welcome to the Task Manager ==========\n" RESET);
    printf(BLUE "      Your personal command-line assistant\n" RESET);

    while (1) {
        int choice;
        printf(CYAN BOLD "\n========== Main Menu ==========\n" RESET);
        printf(GREEN "1. Create a New User\n" RESET);
        printf(GREEN "2. Login\n" RESET);
        printf(YELLOW "3. Show Leaderboard\n" RESET);
        printf(WHITE "4. Exit\n" RESET);
        printf(BOLD "\nEnter your choice: " RESET);

        if (scanf("%d", &choice) != 1) {
            printf(RED "Invalid input. Please enter a number.\n" RESET);
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                addUser();
                break;
            case 2:
                if (login(current_user)) { 
                    printf(GREEN "\n--------------------------------------\n" RESET);
                    write_status(current_user);
                    show_reminders(current_user); 
                    printf(GREEN "--------------------------------------\n" RESET);

                    show_user_menu(current_user); 
                }
                break;
            case 3:
                showLeaderboard();
                break;
            case 4:
                printf(YELLOW "Goodbye! See you next time.\n" RESET);
                return 0; 
            default:
                printf(RED "Invalid choice. Please try again.\n" RESET);
        }
    }
    return 0;
}

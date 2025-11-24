#ifndef COMMON_H
#define COMMON_H

// --- Functions from user.c ---
void addUser();
int login(char* username_out);
void showLeaderboard();
void update_reward_score(const char* username, int points_to_add);

// --- Functions from tasks.c ---
void create_new_tasks(const char* username);
void view_tasks(const char* username);
void update_task(const char* username);
int complete_task(const char* username);
void delete_task(const char* username); // Renamed for clarity
void rearrange_tasks(const char* username); // Renamed for clarity
void show_reminders(const char* username); // Renamed for clarity
void clear_input_buffer(); // This utility is used everywhere

#endif // COMMON_H
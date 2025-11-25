#ifndef COMMON_H
#define COMMON_H

//For colour coded terminal output
#define RESET   "\x1b[0m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1b[37m"
#define BOLD    "\x1b[1m"

// Functions from user.c
void addUser();
int login(char* username_out);
void showLeaderboard();
void update_reward_score(const char* username, int points_to_add);



// Functions from tasks.c
void create_new_tasks(const char* username);
void view_tasks(const char* username);
void update_task(const char* username);
int complete_task(const char* username);
void delete_task(const char* username); 
void rearrange_tasks(const char* username); 
void show_reminders(const char* username); 
void clear_input_buffer(); 
void write_status(const char* username);


#endif

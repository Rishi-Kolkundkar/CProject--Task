#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "sha256_wrapper.h"

#define RESET   "\x1b[0m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1b[37m"


typedef struct {
    int id;
    char title[64];
    char description[256];
    int priority;       // lower => higher priority
    int wtype;     // 1 = personal, 0 = work
    char due_date[11];  // format: YYYY-MM-DD
    char owner[32];     // username of creator
} Task;
int is_valid_date(const char* date) {
    if (strlen(date) != 10 || date[2] != '-' || date[5] != '-') return 0;

    // Extract day, month, year
    int day = (date[0] - '0') * 10 + (date[1] - '0');
    int month = (date[3] - '0') * 10 + (date[4] - '0');
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + (date[8] - '0') * 10 + (date[9] - '0');

    // Basic range checks
    if (day < 1 || month < 1 || month > 12 || year < 1900 || year > 2100) return 0;

    // Days in each month
    int days_in_month[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

    // Leap year check
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        days_in_month[1] = 29;

    if (day > days_in_month[month - 1]) return 0;

    return 1;
}

char* fnamegen(const char* username){
    char fname[64];
    sprintf(fname, "%s_task.txt", username);
    return fname;
}



int create_task(const char* username, int tid) {
    FILE *fh_op;
    char fname[64];
    Task rec;

    strcpy(fname, fnamegen(username));

    rec.id = tid;
    strcpy(rec.owner, username);

    printf("Enter title: ");
    fgets(rec.title, sizeof(rec.title), stdin);
    rec.title[strcspn(rec.title, "\n")] = 0;

    printf("Enter description: ");
    fgets(rec.description, sizeof(rec.description), stdin);
    rec.description[strcspn(rec.description, "\n")] = 0;

    printf("Enter priority: ");
    scanf("%d", &rec.priority);

    printf("Personal  task? (1 = yes, 0 = no): ");
    scanf("%d", &rec.wtype);

    char date[11];
    while (1) {
        printf("Enter due date (YYYY-MM-DD): ");
        scanf("%s", date);
        if (is_valid_date(date)) {
            strcpy(rec.due_date, date);
            break;
        } else {
            printf("Invalid date format. Try again.\n");
        }
    }

    fh_op = fopen(fname, "a");
    if (fh_op == NULL) {
        printf("Error opening file.\n");
        return 0;
    }

    if (fwrite(&rec, sizeof(Task), 1, fh_op) != 1) {
        printf("Error writing to file.\n");
        fclose(fh_op);
        return 0;
    }

    fclose(fh_op);
    return 1;
}



void create_tasks(const char* username) {
    int n;
    printf("Enter number of tasks to be read: ");
    scanf("%d", &n);

    char fname[64];
    strcpy(fname, fnamegen(username));

    FILE* fh = fopen(fname, "rb+"); 
    int existing_total = 0;

    if (fh != NULL) {
        fread(&existing_total, sizeof(int), 1, fh); // read existing count
    } else {
        fh = fopen(fname, "wb+"); // creates a new file
        if (fh == NULL) {
            printf("Error opening file.\n");
            return;
        }
    }

    fseek(fh, 0, SEEK_SET); // go back to start
    int new_total = existing_total + n;
    fwrite(&new_total, sizeof(int), 1, fh); // update total
    
    fclose(fh);

    int i = 0;
    while (i < n) {
        int tid = existing_total + i + 1; // unique ID
        if (create_task(username, tid) == 1) {
            printf("Task %d added successfully.\n", tid);
            i++;
        } else {
            printf("Task creation failed. Try again.\n");
        }
    }
}

void rotate_task_to_front(Task* tasks, int total, int target_id) {
    int idx = -1;

    
    for (int i = 0; i < total; i++) {
        if (tasks[i].id == target_id) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        printf("Task ID not found.\n");
        return;
    }


    Task temp = tasks[idx];

    
    for (int i = idx; i > 0; i--) {
        tasks[i] = tasks[i - 1];
    }

    
    tasks[0] = temp;


    for (int i = 0; i < total; i++) {
        tasks[i].priority = i + 1;
    }
}


void rearrange(const char* username){
    char fname[64];
    strcpy(fname,fnamegen(username));
    
    FILE* fh= fopen(fname,"rb");
    if (fh==NULL) return;
    int total_tasks;
    if(fread(&total_tasks,sizeof(int),1,fh)!=1) {
        printf("File read failed. Try again.\n");
        return;
    }

    Task* data = (Task*) malloc(total_tasks*sizeof(Task));
    if (data==NULL) return;
    if (fread(data,sizeof(Task),total_tasks,fh)!=total_tasks){
        printf("File read failed. Try again.\n");
        return;
    }
    fclose(fh);

    printf("Your current taks are in priority order: \n\n");
    printf(CYAN "%-5s %-20s %-10s %-8s %-12s %-10s\n" RESET,"ID", "Title", "Priority", "Private", "Due Date", "Owner");
    printf("-------------------------------------------------------------------------------------------------");
    for (int i=0;i<total_tasks;i++){
        Task task = data[i];
        if (task.priority == 1) printf(RED);
        else if (task.priority == 2) printf(YELLOW);
        else printf(GREEN);

        printf("%-5d %-20s %-10d %-8s %-12s %-10s\n",
       task.id,
       task.title,
       task.priority,
       task.wtype ? "Personal" : "Work",
       task.due_date,
       task.owner);
    }

    int target_id,ind=-1;
    printf("Enter the ID of the task to update: ");
    scanf("%d", &target_id);
    for (int i=0;i<total_tasks;i++){
        if (data[i].id==target_id) ind = i;
    }
    if (ind==-1) {
        printf("Invalid ID entered\n");
        return;
    }


    int new_priority;
    printf("Enter new priority (1 = highest): ");
    scanf("%d", &new_priority);

    data[ind].priority=new_priority;

    rotate_task_to_front(data,total_tasks,ind);

    FILE* fh = fopen(fname,"wb");
    if (fh==NULL){
        printf("File re-write failed. Try again.\n");
        return;
    }
    fwrite(&total_tasks, sizeof(int), 1, fh);


    for (int i=0;i<total_tasks;i++){
        if (fwrite(&data[i], sizeof(Task), 1, fh) != 1) {
        printf("Error writing to file.\n");
        fclose(fh);
        return 0;
        }
    }
    free(data);
    fclose(fh);

}


void delete(const char* username){
    char fname[64];
    strcpy(fname,fnamegen(username));
    
    FILE* fh= fopen(fname,"rb");
    if (fh==NULL) return;
    int total_tasks;
    if(fread(&total_tasks,sizeof(int),1,fh)!=1) {
        printf("File read failed. Try again.\n");
        return;
    }

    Task* data = (Task*) malloc(total_tasks*sizeof(Task));
    if (data==NULL) return;
    if (fread(data,sizeof(Task),total_tasks,fh)!=total_tasks){
        printf("File read failed. Try again.\n");
        return;
    }
    fclose(fh);

    printf("Your current taks are in priority order: \n\n");
    printf(CYAN "%-5s %-20s %-10s %-8s %-12s %-10s\n" RESET,"ID", "Title", "Priority", "Private", "Due Date", "Owner");
    printf("-------------------------------------------------------------------------------------------------");
    for (int i=0;i<total_tasks;i++){
        Task task = data[i];
        if (task.priority == 1) printf(RED);
        else if (task.priority == 2) printf(YELLOW);
        else printf(GREEN);

        printf("%-5d %-20s %-10d %-8s %-12s %-10s\n",
       task.id,
       task.title,
       task.priority,
       task.wtype ? "Personal" : "Work",
       task.due_date,
       task.owner);
    }

    

    int target_id,ind=-1;
    printf("Enter the ID of the task to delete: ");
    scanf("%d", &target_id);
    for (int i=0;i<total_tasks;i++){
        if (data[i].id==target_id) ind = i;
    }

    if (ind==-1) {
        printf("Invalid ID entered\n");
        return;
    }

    for (int i = ind; i < total_tasks - 1; i++) {
        data[i] = data[i + 1];
    }

    total_tasks--;

    data = realloc(data,total_tasks*sizeof(Task));

    FILE* fh = fopen(fname,"wb");
    if (fh==NULL){
        printf("File re-write failed. Try again.\n");
        return;
    }

    fwrite(&total_tasks, sizeof(int), 1, fh);

    for (int i=0;i<total_tasks;i++){
        // Reassign priorities
        data[i].priority=i+1;
        if (fwrite(&data[i], sizeof(Task), 1, fh) != 1) {
        printf("Error writing to file.\n");
        fclose(fh);
        return 0;
        }
    }

    free(data);
    fclose(fh);

}



int login(char* username_out) {
    char input_user[32], input_pass[32];
    int attempts = 0;

    while (attempts < 3) {
        printf("Username: ");
        scanf("%s", input_user);
        printf("Password: ");
        scanf("%s", input_pass);

        FILE* fp = fopen("users.txt", "r");
        if (fp == NULL) {
            printf("User database missing.\n");
            return 0;
        }

        char file_user[32], file_pass[32];
        int success = 0;
        while (fscanf(fp, "%s %s", file_user, file_pass) == 2) {
            if (strcmp(input_user, file_user) == 0 &&
                strcmp(input_pass, file_pass) == 0) {
                strcpy(username_out, input_user);
                success = 1;
                break;
            }
        }
        fclose(fp);

        if (success) {
            printf("Login successful. Welcome, %s!\n", username_out);
            return 1;
        } else {
            attempts++;
            printf("Invalid credentials. Attempt %d of 3.\n", attempts);
        }
    }

    printf("Too many failed attempts. Access denied.\n");
    return 0;
}



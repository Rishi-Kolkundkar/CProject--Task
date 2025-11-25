#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define RESET   "\x1b[0m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1b[37m"

typedef enum {
    TASK_PENDING,
    TASK_COMPLETED,
    TASK_OVERDUE
} TaskStatus;

typedef struct {
    int id;
    char title[64];
    char description[256];
    int priority; 
    int wtype;
    char due_date[11];
    char owner[32];
    TaskStatus status; // <-- ADD THIS FIELD
} Task;


void set_status(Task* task);

//Helper Functions
int is_valid_date(const char* date) {
    if (strlen(date) != 10 || date[4] != '-' || date[7] != '-') return 0;

    int year, month, day;
    // Use sscanf to parse in YYYY-MM-DD format
    if (sscanf(date, "%d-%d-%d", &year, &month, &day) != 3) return 0;

    // Basic range checks
    if (year < 1900 || year > 2100 || month < 1 || month > 12 || day < 1) return 0;

    // Days in each month
    int days_in_month[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // Leap year check
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        days_in_month[2] = 29;
    }

    if (day > days_in_month[month]) return 0;

    return 1; // The date is valid
}

void fnamegen(const char* username, char* fname_out, size_t size) {
    snprintf(fname_out, size, "%s_task.txt", username);
}

char* revstr(char *str) {
    if (str == NULL) {
        return NULL;
    }
    int length = strlen(str);
    char *start = str;
    char *end = str + length - 1;
    while (end > start) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
    return str;
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void current_date(char *current_day, char *current_month, char *current_year){
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    int day = t->tm_mday;
    int month = t->tm_mon+1;
    int year = t->tm_year+1900;
    sprintf(current_day, "%02d", day);
    sprintf(current_month, "%02d", month);
    sprintf(current_year, "%d", year);
}

int dayOfWeek(int day, int month, int year) {
    if (month < 3){
        month += 12;
        year -= 1;
    }
    int k = year%100; // Year within the century
    int j = year/100; // Century
    int day_of_week = (day + (13 *(month + 1))/5 + k + (k/4) + (j/4) - (2*j)) % 7; //13*.. assinging weights(January-1, etc) 
    return (day_of_week + 7)%7; //k/4 leap year j/4 century leap year; 2j century correction
}

char *day_name_convert(int day, int month, int year){
    char *days[] = {"Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    int day1 = dayOfWeek(day, month, year);
    return days[day1];
}

int LeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int days_in_month(int month, int year){
    switch(month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            return LeapYear(year) ? 29 : 28;
        default:
            return -1; // Invalid month
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




//Functions which actually peform tasks



void create_new_tasks(const char* username) {   
    char fname[64];
    fnamegen(username, fname, sizeof(fname));
    
    
    FILE* fh = fopen(fname, "rb");
    int total_tasks = 0;
    Task* tasks = NULL;

    if (fh != NULL) {
       
        fread(&total_tasks, sizeof(int), 1, fh);
        if (total_tasks > 0) {
            tasks = malloc(total_tasks * sizeof(Task));
            fread(tasks, sizeof(Task), total_tasks, fh);
        }
        fclose(fh);
    }


    int n;
    printf("How many new tasks to create? ");
    while(scanf("%d", &n)!=1){
        clear_input_buffer();
        printf("Invalid input\n");
        printf("How many new tasks to create? ");
    }
    if (n <= 0) return;

    int new_total = total_tasks + n;
    
    tasks = realloc(tasks, new_total * sizeof(Task));

    for (int i = 0; i < n; i++) {
        Task* new_task = &tasks[total_tasks + i]; 

        printf("\n--- Creating Task %d of %d ---\n", i + 1, n);
        
        new_task->id = total_tasks + i + 1; 
        strcpy(new_task->owner, username);
        //new_task->status = TASK_PENDING;
        set_status(new_task);

        printf("Enter title: ");
        fgets(new_task->title, sizeof(new_task->title), stdin);
        new_task->title[strcspn(new_task->title, "\n")] = 0;

        printf("Enter description: ");
        fgets(new_task->description, sizeof(new_task->description), stdin);
        new_task->description[strcspn(new_task->description, "\n")] = 0;
        
        printf("Enter priority (1-High, 2-Medium, 3-Low): ");
        scanf("%d", &new_task->priority);
        clear_input_buffer();

        printf("Personal task? (1=yes, 0=no): ");
        scanf("%d", &new_task->wtype);
        clear_input_buffer();
        
        while (1) {
            printf("Enter due date (YYYY-MM-DD): ");
            scanf("%10s", new_task->due_date);
            clear_input_buffer();

            if (is_valid_date(new_task->due_date)) {
                break;
            } else {
                printf("Invalid date format. Please use YYYY-MM-DD.\n");
            }
        }
    }

   
    fh = fopen(fname, "wb");
    if (fh == NULL) {
        printf("Error: Could not save tasks.\n");
        free(tasks);
        return;
    }
    
    
    fwrite(&new_total, sizeof(int), 1, fh);
   
    fwrite(tasks, sizeof(Task), new_total, fh);
    
    fclose(fh);
    free(tasks);
    printf("\nSuccessfully added %d new task(s).\n", n);
}




void rearrange_tasks(const char* username) {
    char fname[64];
    fnamegen(username, fname, sizeof(fname));

   
    FILE* fh = fopen(fname, "rb");
    if (fh == NULL) {
        printf("No tasks to rearrange.\n");
        return;
    }

    int total_tasks = 0;
    if (fread(&total_tasks, sizeof(int), 1, fh) != 1 || total_tasks == 0) {
        printf("You have no tasks to rearrange.\n");
        fclose(fh);
        return;
    }

    Task* tasks = (Task*)malloc(total_tasks * sizeof(Task));
    if (tasks == NULL) { fclose(fh); return; }
    if (fread(tasks, sizeof(Task), total_tasks, fh) != total_tasks) {
        free(tasks); fclose(fh); return;
    }
    fclose(fh);

   
    printf("Your current tasks are:\n");
    printf(CYAN "%-5s %-20s %-10s\n" RESET, "ID", "Title", "Priority");
    printf("----------------------------------------\n");
    for (int i = 0; i < total_tasks; i++) {
        printf("%-5d %-20s %-10d\n", tasks[i].id, tasks[i].title, tasks[i].priority);
    }
    printf(RESET);

 
    printf("\nEnter the ID of the task to reprioritize: ");
    int target_id;
    scanf("%d", &target_id);
    clear_input_buffer();

    printf("Enter the new priority number (1 is highest): ");
    int new_priority;
    scanf("%d", &new_priority);
    clear_input_buffer();

    // Validate inputs
    if (new_priority < 1 || new_priority > total_tasks) {
        printf(RED "Error: Invalid priority number. It must be between 1 and %d.\n" RESET, total_tasks);
        free(tasks);
        return;
    }

    int old_index = -1;
    for (int i = 0; i < total_tasks; i++) {
        if (tasks[i].id == target_id) {
            old_index = i;
            break;
        }
    }

    if (old_index == -1) {
        printf(RED "Error: Task with ID %d not found.\n" RESET, target_id);
        free(tasks);
        return;
    }

    
    // Making a copy of the task we are moving
    Task task_to_move = tasks[old_index];
    int new_index = new_priority - 1; // Converting the 1-based priority to 0-based index

    
    if (old_index > new_index) {
       
        // Shift tasks between new_index and old_index downward manner
        for (int i = old_index; i > new_index; i--) {
            tasks[i] = tasks[i - 1];
        }
    } else if (old_index < new_index) {
       
        // Shift tasks between old_index and new_index upward manner
        for (int i = old_index; i < new_index; i++) {
            tasks[i] = tasks[i + 1];
        }
    }
    
    
    tasks[new_index] = task_to_move;

    
    for (int i = 0; i < total_tasks; i++) {
        tasks[i].priority = i + 1;
    }


    fh = fopen(fname, "wb");
    if (fh == NULL) {
        printf(RED "CRITICAL ERROR: Could not open file to save changes.\n" RESET);
        free(tasks);
        return;
    }

    if (fwrite(&total_tasks, sizeof(int), 1, fh) != 1) { /* handle error */ }
    
    if (fwrite(tasks, sizeof(Task), total_tasks, fh) != total_tasks) {
        printf(RED "CRITICAL ERROR: Failed to write all tasks to file. Data may be corrupt.\n" RESET);
    } else {
        printf(GREEN "Task priorities updated successfully.\n" RESET);
    }

    fclose(fh);
    free(tasks);
}

void set_status(Task* task){
    char current_day_str[3], current_month_str[3], current_year_str[5];
    current_date(current_day_str, current_month_str, current_year_str);
    int current_year = atoi(current_year_str);
    int current_month = atoi(current_month_str);
    int current_day = atoi(current_day_str);

    int task_year, task_month, task_day;
    // FIX: Parse in YYYY-MM-DD order
    sscanf(task->due_date, "%d-%d-%d", &task_year, &task_month, &task_day);

    // If the task is already completed, do nothing.
    if(task->status == TASK_COMPLETED) {
        return;
    }

    // Check if the date is in the past.
    if (task_year < current_year || 
       (task_year == current_year && task_month < current_month) || 
       (task_year == current_year && task_month == current_month && task_day < current_day)) 
    {
        task->status = TASK_OVERDUE;
    } else {
        task->status = TASK_PENDING;
    }
}

void write_status(const char* username) {
    char fname[64]; 
    fnamegen(username, fname, sizeof(fname));
    
    FILE *fh = fopen(fname, "rb");
    if (fh == NULL) {
        perror("Error");
        return;
    }
    int total_tasks;
    if (fread(&total_tasks, sizeof(int), 1, fh) != 1) {
        fclose(fh);
        return;
    }
    if(total_tasks==0){
        printf("You haven't created any tasks!\n");
        return;
    }
    Task* data = (Task*) malloc(total_tasks * sizeof(Task));
    fread(data, sizeof(Task), total_tasks, fh);
    fclose(fh); 

    
    int changed = 0;
    for (int i = 0; i < total_tasks; i++) {
        TaskStatus old_status = data[i].status;
        set_status(&data[i]); //Update based on today's date
        
        if (data[i].status != old_status) changed = 1;
    }

    // Only write to file if something changed
    if(changed){
        fh = fopen(fname, "wb"); // Open write mode
        fwrite(&total_tasks, sizeof(int), 1, fh);
        fwrite(data, sizeof(Task), total_tasks, fh);
        fclose(fh);
    }

    free(data);
}

void view_tasks(const char *username){
    //on user selecting option to view tasks give 3 options day wise, week wise and by month
    Task *data = NULL;
    char fname[64]; 
    fnamegen(username, fname, sizeof(fname));
    FILE *fh = NULL;
    fh = fopen(fname, "rb");
    if(fh==NULL){
        perror("Error");
        return;
    }
    int total_tasks;
    if(fread(&total_tasks,sizeof(int),1,fh)!=1){
        printf("File read failed or file is empty. Try again.\n");
        goto cleanup;
    }
    if(total_tasks==0){
        printf("You haven't created any tasks!\n");
        goto cleanup;
    }
    data = (Task*) malloc(total_tasks*sizeof(Task));
    if (data==NULL) {
        printf("Memory allocation failed.\n");
        goto cleanup;
    }
    if (fread(data,sizeof(Task),total_tasks,fh)!=total_tasks){
        printf("File read failed. Try again.\n");
        goto cleanup;
    }

    printf("******************************\n");

    //First output in function
    printf("Which view would you like? Daily(1) Weekly(2) Monthly(3)\n");
    int temp = 0;
    if(scanf("%d", &temp)!=1) {
        clear_input_buffer();
        temp = -1; 
    }
    //Get current date
    char current_day[3], current_month[3], current_year[5];
    current_date(current_day, current_month, current_year);
    int current_year_int = atoi(current_year);
    int current_month_int = atoi(current_month);
    int current_day_int = atoi(current_day);

    //YYYY-MM-DD Task->due_date    
    //Need to check deadline of each tasks --> if it is due today
    if(temp==1){
        //If there are no tasks due today print the same
        int flag = 0, flag1 = 0;
        for(int i = 0; i<total_tasks; i++){
            set_status(&data[i]);
            if(strncmp(data[i].due_date, current_year, 4) == 0 &&
                strncmp(data[i].due_date + 5, current_month, 2) == 0 &&
                strncmp(data[i].due_date + 8, current_day, 2) == 0){
                int task_year, task_month, task_day;
                sscanf(data[i].due_date, "%d-%d-%d", &task_year, &task_month, &task_day);
                if(flag1==0){
                    printf("Today(%s, %02d-%02d-%04d) you have the following tasks: \n", day_name_convert(current_day_int, current_month_int, current_year_int), task_day, task_month, task_year);
                    flag1 = 1;
                }
                if (data[i].priority == 1) printf(RED);
                else if (data[i].priority == 2) printf(YELLOW);
                else printf(GREEN);
                printf("⦿ %s(%d)   %s-> STATUS: %s%s%s\n\t⇥ %s\n", data[i].title, data[i].priority, RESET, data[i].status==TASK_COMPLETED?GREEN : YELLOW, data[i].status==TASK_COMPLETED? "COMPLETED!" : "PENDING", RESET, data[i].description); //Colour code title based on priority; priority marked as int for now, needs to be changed to string
                if(data[i].wtype==1) printf("\t⇥ Type: Personal\n");
                else printf("\t⇥ Type: Work\n");
                flag = 1;
            }
        }
        if(flag==0){
            printf("You have no tasks due today!\n");
            printf("Returning to the main menu....\n");
            goto cleanup;
        }
    }
    else if(temp==2){
        char* days_of_week[] = {"Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
        for (int day_num =0; day_num <7; day_num++) {
            printf("%-9s -->", days_of_week[day_num]); //-9s is for alignment
            printf("\n");   
            int tasks_on_day = 0;
            for (int i = 0; i < total_tasks; i++) {
                set_status(&data[i]);
                int task_year, task_month, task_day;
                sscanf(data[i].due_date, "%d-%d-%d", &task_year, &task_month, &task_day);

                char *str_day_task = day_name_convert(task_day, task_month, task_year);

                // Check if the task's due date matches current day
                if (strcmp(str_day_task, days_of_week[day_num]) == 0  && task_year==current_year_int && 
                    task_month==current_month_int && task_day<current_day_int + 7)
                {
                    //if (tasks_on_day == 0) { printf("\n"); }            
                    // Set color based on priority
                    if (data[i].priority == 1) printf(RED);
                    else if (data[i].priority == 2) printf(YELLOW);
                    else printf(GREEN);
                    
                    // Print task details with indentation
                    printf("\t\t\t  ⦿ %s (%d)", data[i].title, data[i].priority);
                    if(data[i].status==TASK_OVERDUE) printf(RED "   -> STATUS: OVERDUE\n" RESET);
                    else if(data[i].status==TASK_COMPLETED) printf(GREEN "   -> STATUS: COMPLETED!\n" RESET);
                    else printf(YELLOW "   -> STATUS: PENDING\n" RESET);
                    
                    printf("\t\t\t\t⇥ %s\n", data[i].description);
                    if (data[i].wtype == 1) printf("\t\t\t\t⇥ Type: Personal\n");
                    else printf("\t\t\t\t⇥ Type: Work\n");

                    // Check for overdue
                    
                    printf(RESET "\n"); // Add a blank line after each task
                    tasks_on_day++;
                }
            }
            //if no tasks on this day
            if (tasks_on_day == 0) printf("You have no tasks on this day!\n"); 
            printf("\n"); //new line after a day
        }
    }
    else if(temp==3){
        int flag = 0;
        bool has_task[32] = {false};
        TaskStatus day_status[32] = {TASK_PENDING};
        for (int i = 0; i < total_tasks; i++) {
            set_status(&data[i]);
            int task_year, task_month, task_day;
            sscanf(data[i].due_date, "%d-%d-%d", &task_year, &task_month, &task_day);
            if(task_month==current_month_int  && task_year==current_year_int){
                has_task[task_day] = true;
                flag = 1;
                if (data[i].status == TASK_OVERDUE) day_status[task_day] = TASK_OVERDUE;
                else if (data[i].status == TASK_PENDING && day_status[task_day] != TASK_OVERDUE) day_status[task_day] = TASK_PENDING;
            }
        }

        //printing month calendar
        int day_code = dayOfWeek(1, current_month_int, current_year_int);
        int first_day_of_week = (day_code == 0) ? 6 : day_code - 1; // Convert (0=Sat, 1=Sun) to (Sun=0, ..., Sat=6) 
        int days_of_month = days_in_month(current_month_int, current_year_int);

        const char* months[] = {"", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
        printf("\n      <<   %s %s   >>\n\n", months[current_month_int], current_year);
        printf(" Sun   Mon   Tue   Wed   Thu   Fri   Sat\n");
        for (int i = 0; i < first_day_of_week; i++) {
            printf("      "); //spaces based on starting day
        }

        if(flag==0){
            printf("There are no tasks in this month.\n");
            printf("Returning to the main menu....\n");
            goto cleanup;
        }

        for (int day = 1; day<=days_of_month; day++) {
            // Print the day with an asterisk if it has task
            if (has_task[day]) {
                if(day_status[day]==TASK_COMPLETED) printf(GREEN);
                else if(day_status[day]==TASK_OVERDUE) printf(RED);
                else printf(YELLOW);
                printf( " %3d* ", day);
                printf(RESET);
            }
            else printf(" %3d  ", day);

            // If it's Saturday print a newline
            if ((first_day_of_week + day) % 7 == 0) printf("\n"); 
        }
        printf("\n\n");

        printf("--- Tasks Marked with [*] ---\n");
        for(int day = 1; day<=days_of_month; day++){
            if(has_task[day]){
                for(int i = 0; i<total_tasks; i++){
                    int task_year, task_month, task_day;
                    sscanf(data[i].due_date, "%d-%d-%d", &task_year, &task_month, &task_day);
                    if(task_day==day && task_month==current_month_int && task_year==current_year_int){
                        if (data[i].priority == 1) printf(RED);
                        else if (data[i].priority == 2) printf(YELLOW);
                        else printf(GREEN);
                        printf("⦿ [%02d-%02d-%04d] %s\tSTATUS: %s%s  %s\n", task_day, task_month, task_year, data[i].status == TASK_COMPLETED ? GREEN :
                       data[i].status == TASK_PENDING   ? YELLOW : RED,
                       data[i].status == TASK_COMPLETED ? "COMPLETED!" : data[i].status == TASK_PENDING ?"PENDING" :"OVERDUE",RESET, data[i].title);
                    }
                }
            }
        }

        printf(RESET);
    }
    else{
        printf("Invalid entry! Returning to main menu...\n");
        /*printf("Invalid entry! Please try again.\n");
        printf("Would you like to view tasks(0) or exit to main menu(1)?\n");
        int tem = 0;
        scanf("%d", &tem);
        if(tem==0) view_tasks(username);
        else if(tem==1){
            printf("Returning to the main menu....\n");
            return;
        }
        else{
            printf("Whoops! You entered incorrect again!\n");
            printf("Returning to the main menu....\n");
            return;
        }*/
    }

    cleanup:
    if (data != NULL) {
        free(data);
    }
    if (fh != NULL) {
        fclose(fh);
    }
}

void update_task(const char *username){
    Task *data = NULL;
    char fname[64]; 
    fnamegen(username, fname, sizeof(fname));
    FILE *fh = NULL;
    fh = fopen(fname, "rb");
    if(fh==NULL){
        perror("Error");
        return;
    }
    int total_tasks;
    if(fread(&total_tasks,sizeof(int),1,fh)!=1){
        printf("File read failed or file is empty. Try again.\n");
        goto cleanup;
    }

    if(total_tasks==0){
        printf("You haven't created any tasks!\n");
        printf("Returning to main menu...\n");
        goto cleanup;
    }

    data = (Task*) malloc(total_tasks*sizeof(Task));
    if (data==NULL) {
        printf("Memory allocation failed.\n");
        goto cleanup;
    }
    if (fread(data,sizeof(Task),total_tasks,fh)!=total_tasks){
        printf("File read failed. Try again.\n");
        goto cleanup;
    }
    fclose(fh); //Close for reading, will open for writing later
    fh = NULL;

    char current_day[3], current_month[3], current_year[5];
    current_date(current_day, current_month, current_year);
    int current_year_int = atoi(current_year);
    int current_month_int = atoi(current_month);
    int current_day_int = atoi(current_day);

    printf("******************************\n");

    //First need to print what tasks the user has
    printf("Which task would you like to update?\n\n");
    for(int i = 0; i<total_tasks; i++){
        int task_year, task_month, task_day;
        sscanf(data[i].due_date, "%d-%d-%d", &task_year, &task_month, &task_day);
 //DD-MM-YYYY format

        printf(RESET);
        if (data[i].priority == 1) printf(RED);
        else if (data[i].priority == 2) printf(YELLOW);
        else printf(GREEN);

        printf("[%d] %s ⇥ Due: %02d-%02d-%04d, Priority: %d", i+1, data[i].title, task_day, task_month, task_year, data[i].priority);   //putting priority as int but needs to be changed to high medium low
        if(data[i].status==TASK_OVERDUE) printf(RED "\t-> STATUS: OVERDUE\n" RESET);
        else if(data[i].status==TASK_COMPLETED) printf(GREEN "\t-> STATUS: COMPLETED!\n" RESET);
        else printf(YELLOW "\t-> STATUS: PENDING\n" RESET);

        
        if(data[i].wtype==1) printf("\t\t Type: Personal\n");
        else printf("\t\t Type: Work\n");

        
    
    }
    printf(RESET);
    printf("\n");

    printf("Enter the task number to update: (0 to cancel and exit)\n");
    int task_choice = -1;
    //this temp is i+1 bascially so task is data[task_index]
    if (scanf("%d", &task_choice) != 1) {
        clear_input_buffer();
        task_choice = -1; // 
    }
    if(task_choice==0){
        printf("Update cancelled. Returning to main menu...\n");
        goto cleanup;
    }


    while(task_choice>total_tasks || task_choice<0){
        printf("Invalid input! Please try again.\n"); //handling bad input
        printf("Enter the task number to update: \n");
        scanf("%d", &task_choice);
        clear_input_buffer();
        if(task_choice==0){
            printf("Update cancelled. Returning to main menu...\n");
            goto cleanup;
        }
    }
    //printing details of selected task:
    int task_index = task_choice-1;
    bool change_made = false;

    while(1){
        if (data[task_index].priority == 1) printf(RED);
        else if (data[task_index].priority == 2) printf(YELLOW);
        else printf(GREEN);
        printf("--- Editing Task [%d]: %s ---", task_choice, data[task_index].title);
        printf(RESET);
        printf("\n");
        printf("[1] %-12s %s\n", "Title:", data[task_index].title);
        printf("[2] %-12s %s\n", "Description:", data[task_index].description);
        printf("[3] %-12s %s\n", "Due Date:", data[task_index].due_date);
        printf("[4] %-12s %d\n", "Priority:", data[task_index].priority);
        if(data[task_index].wtype==1) printf("[5] %-12s Personal\n", "Type:");
        else printf("[5] %-12s Work\n", "Type:");

        int field_choice = -1;
        while(1){
            printf("\nWhich field would you like to change? (Enter 0 to save and exit)\n");
            if (scanf("%d", &field_choice) !=1){
                printf("Invalid input! Please enter a number.\n");
                clear_input_buffer(); 
                continue; 
            }
            clear_input_buffer();

            if(field_choice>=0 && field_choice<=5) break;
            else printf("Invalid option! Please enter 0-5.\n");
        }


        if(field_choice==0){
            printf("Saved succesfully!\n");
            break;
        }

        change_made = true;
        
        switch(field_choice){
            case 1: // Title
                printf("Enter new title: ");
                fgets(data[task_index].title, sizeof(data[task_index].title), stdin); //Can't use scanf here as it breaks on encountering space
                data[task_index].title[strcspn(data[task_index].title, "\n")] = 0; //Removing newline
                break;
            case 2: // Description
                printf("Enter new description: ");
                fgets(data[task_index].description, sizeof(data[task_index].description), stdin);
                data[task_index].description[strcspn(data[task_index].description, "\n")] = 0; 
                break;
            case 3: // due date

                while(1) { //loop till correct input
                    char temp_date[20];
                    printf("Enter new due date (YYYY-MM-DD): ");
                    fgets(temp_date, sizeof(temp_date), stdin);
                    temp_date[strcspn(temp_date, "\n")] = 0; 
                    if(is_valid_date(temp_date)==1) {
                        strcpy(data[task_index].due_date, temp_date);
                        break;
                    }
                    else printf("Invalid date format! Please use YYYY-MM-DD.\n");
                }

                break;
            case 4: // Priority

                while(1){
                    printf("Enter new priority (1-High, 2-Medium, 3-Low): ");
                    if(scanf("%d", &data[task_index].priority) != 1){
                        printf("Invalid input! Enter a number.\n");
                        clear_input_buffer(); // clear bad input "a"
                        continue;
                    }
                    clear_input_buffer(); //removing newline
                    if (data[task_index].priority>=1 && data[task_index].priority <= 3) break;
                    else printf("Invalid priority! Please enter 1, 2, or 3.\n");
                }

                break;
            case 5: // Type

                while (1){
                    printf("Enter new type (1-Personal, 0-Work): ");
                    if(scanf("%d", &data[task_index].wtype) != 1) {
                        printf("Invalid input! Enter a number.\n");
                        clear_input_buffer(); 
                        continue;
                    }
                    clear_input_buffer();
                    if (data[task_index].wtype == 0 || data[task_index].wtype == 1) break;
                    else printf("Invalid type! Please enter 0 or 1.\n");
                }

                break;
        }

    //end of while
    }

    if(change_made==true){
        printf("\nSaving changes...\n");
        fh = fopen(fname, "wb");
        if(fh==NULL){
            perror("Error: Could not open file to save changes");
            goto cleanup;
        }

        //Deleting all contents and re-writing file
        if(fwrite(&total_tasks, sizeof(int), 1, fh)!=1){
            printf("Error writing task count to file.\n");
            goto cleanup;
        }
        if(fwrite(data, sizeof(Task), total_tasks, fh)!=total_tasks) {
            printf("Error writing tasks to file.\n");
            goto cleanup;
        }
        printf("Succesfully saved changes!\n");
    }
    else printf("No changes were made.\n");

    printf("Returning to main menu...\n");

    cleanup:
    if (data != NULL) {
        free(data);
    }
    if (fh != NULL) {
        fclose(fh);
    }
}



void delete_task(const char* username){
     char fname[64]; 
    fnamegen(username, fname, sizeof(fname));
    
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
    fh=NULL;

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

    fh = fopen(fname,"wb");
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
        return ;
        }
    }

    free(data);
    fclose(fh);

}

int complete_task(const char* username) {
    char fname[64];
    fnamegen(username, fname, sizeof(fname));

    // --- Step 1: Read all tasks from the file into memory ---
    FILE* fh = fopen(fname, "rb");
    if (fh == NULL) {
        printf("You have no tasks to complete.\n");
        return 0;
    }

    int total_tasks = 0;
    // Read the total number of tasks
    if (fread(&total_tasks, sizeof(int), 1, fh) != 1 || total_tasks == 0) {
        printf("You have no tasks to complete.\n");
        fclose(fh);
        return 0;
    }

    // Allocate memory for all tasks
    Task* tasks = (Task*)malloc(total_tasks * sizeof(Task));
    if (tasks == NULL) {
        printf("Memory allocation failed.\n");
        fclose(fh);
        return 0;
    }

    // Read all task data into the allocated memory
    if (fread(tasks, sizeof(Task), total_tasks, fh) != total_tasks) {
        printf("Failed to read all task data from file.\n");
        free(tasks);
        fclose(fh);
        return 0;
    }
    fclose(fh); // Close the file for reading

    // --- Step 2: Get user input ---
    printf("Enter the ID of the task you wish to complete: ");
    int task_id_to_complete;
    if (scanf("%d", &task_id_to_complete) != 1) {
        clear_input_buffer();
        printf("Invalid ID format.\n");
        free(tasks);
        return 0;
    }
    clear_input_buffer();

    // --- Step 3: Find the task and validate its status ---
    int task_index = -1;
    for (int i = 0; i < total_tasks; i++) {
        if (tasks[i].id == task_id_to_complete) {
            task_index = i;
            break;
        }
    }

    if (task_index == -1) {
        printf("Error: Task with ID %d not found.\n", task_id_to_complete);
        free(tasks);
        return 0;
    }

    if (tasks[task_index].status == TASK_COMPLETED) {
        printf("Info: Task '%s' is already marked as complete.\n", tasks[task_index].title);
        free(tasks);
        return 0; // No points for completing it again
    }

    // --- Step 4: Update status in memory and calculate points ---
    // First, check if the task is overdue BEFORE marking it complete
    set_status(&tasks[task_index]); 
    
    int points_earned = 0;
    if (tasks[task_index].status == TASK_OVERDUE) {
        printf("Completed an overdue task. Better late than never!\n");
        points_earned = 1; // Minimal points for late completion
    } else {
        printf("Task completed on time! Great job!\n");
        points_earned = 10; // Base points for on-time completion
        if (tasks[task_index].priority == 1) {
            points_earned += 5; // Bonus for high-priority
            printf("Bonus points for completing a high-priority task!\n");
        }
    }

    // Now, permanently mark the task's status as completed in memory
    tasks[task_index].status = TASK_COMPLETED;

    // --- Step 5: Write the entire modified task list back to the file ---
    fh = fopen(fname, "wb");
    if (fh == NULL) {
        printf("CRITICAL ERROR: Could not open file to save changes.\n");
        free(tasks);
        return 0; // Return 0 points because the change was not saved
    }

    // Write the total count first
    fwrite(&total_tasks, sizeof(int), 1, fh);
    // Write the entire array of tasks
    fwrite(tasks, sizeof(Task), total_tasks, fh);
    fclose(fh);

    printf("Task '%s' marked as complete! You earned %d points.\n", tasks[task_index].title, points_earned);
    
    // --- Step 6: Cleanup ---
    free(tasks);
    return points_earned;
}

void show_reminders(const char *username){
    Task *data = NULL;
    char fname[64]; 
    fnamegen(username, fname, sizeof(fname));
    FILE *fh = NULL;
    fh = fopen(fname, "rb");
    if(fh==NULL){
        perror("Error");
        return;
    }
    int total_tasks;
    if(fread(&total_tasks,sizeof(int),1,fh)!=1){
        printf("File read failed or file is empty. Try again.\n");
        goto cleanup;
    }

    if(total_tasks==0){
        printf("You haven't created any tasks!\n");
        printf("Returning to main menu...\n");
        goto cleanup;
    }

    data = (Task*) malloc(total_tasks*sizeof(Task));
    if (data==NULL) {
        printf("Memory allocation failed.\n");
        goto cleanup;
    }
    if (fread(data,sizeof(Task),total_tasks,fh)!=total_tasks){
        printf("File read failed. Try again.\n");
        goto cleanup;
    }

    char current_day[3], current_month[3], current_year[5];
    current_date(current_day, current_month, current_year);
    int current_year_int = atoi(current_year);
    int current_month_int = atoi(current_month);
    int current_day_int = atoi(current_day);


    //put immediately after login
    printf("**************************************\n");
    printf("*       🔔   REMINDERS   🔔         *\n");
    printf("**************************************\n");

    //Looping through the tasks to see which task is due when

    //Overdue
    printf(RED);
    printf("🚨 [OVERDUE]\n");
    printf(RESET);
    int flag_o = 0;
    for(int i = 0; i<total_tasks; i++){

        int task_year, task_month, task_day;
        sscanf(data[i].due_date, "%d-%d-%d", &task_year, &task_month, &task_day);
        if(task_year < current_year_int || (task_year == current_year_int && task_month < current_month_int) || (task_year == current_year_int && task_month == current_month_int && task_day < current_day_int)){
            flag_o = 1;
            printf("%s (Due: %02d-%02d-%04d)\n", data[i].title, task_day, task_month, task_year);
        }
    }
    if(flag_o==0) printf("Good to go!\n");
    printf("\n");

    //Due today
    printf(YELLOW);
    printf("⏳ [DUE TODAY]\n");
    printf(RESET);
    int flag_t = 0;
    for(int i = 0; i<total_tasks; i++){

        int task_year, task_month, task_day;
        sscanf(data[i].due_date, "%d-%d-%d", &task_year, &task_month, &task_day);

        if(current_day_int==task_day && current_month_int==task_month && current_year_int==task_year){
            flag_t = 1;
            printf("%s (Due: %02d-%02d-%04d)\n", data[i].title, task_day, task_month, task_year);
        }
    }

    if(flag_t==0) printf("Good to go!\n");
    printf("\n");

    printf("Select 'View Tasks' for more details.\n");

    cleanup:
    if (data != NULL) {
        free(data);
    }
    if (fh != NULL) {
        fclose(fh);
    }

}














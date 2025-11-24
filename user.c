// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// #define FILE_NAME "users.txt"

// typedef struct {
//     char name[50];
//     char password[50];
//     int reward;
// } User;

// // void addUser();
// // void loadUsers(User **list, int *count);
// // int login(User *list, int count);
// // void showProfile(User u);
// // void showCurrentUser(User *u);
// // void showLeaderboard();


// void addUser() {
//     FILE *fp = fopen(FILE_NAME, "a");
//     if (!fp) {
//         printf("Cannot open file.\n");
//         return;
//     }

//     User u;
//     printf("Enter username: ");
//     scanf("%s", u.name);
//     printf("Enter password: ");
//     scanf("%s", u.password);

//     u.reward = 0;

//     fprintf(fp, "%s %s %d\n", u.name, u.password, u.reward);
//     fclose(fp);

//     printf("User created.\n");
// }

// void loadUsers(User **list, int *count) {
//     FILE *fp = fopen(FILE_NAME, "r");
//     if (!fp) {
//         *count = 0;
//         return;
//     }

//     int c = 0;
//     char a[50], b[50];
//     int r;

//     while (fscanf(fp, "%s %s %d", a, b, &r) == 3)
//         c++;

//     rewind(fp);

//     *list = malloc(sizeof(User) * c);
//     *count = c;

//     for (int i = 0; i < c; i++) {
//         fscanf(fp, "%s %s %d", 
//                (*list)[i].name,
//                (*list)[i].password,
//                &(*list)[i].reward);
//     }

//     fclose(fp);
// }

// int login(User *list, int count) {
//     char n[50], p[50];
//     printf("Username: ");
//     scanf("%s", n);
//     printf("Password: ");
//     scanf("%s", p);

//     for (int i = 0; i < count; i++) {
//         if (strcmp(list[i].name, n) == 0 &&
//             strcmp(list[i].password, p) == 0) {
//             printf("Login successful.\n");
//             return i;
//         }
//     }

//     printf("Wrong username or password.\n");
//     return -1;
// }

// void showProfile(User u) {
//     printf("\n--- PROFILE ---\n");
//     printf("Name: %s\n", u.name);
//     printf("Reward: %d\n", u.reward);
//     printf("---------------\n");
// }

// void showCurrentUser(User *u) {
//     if (u == NULL) {
//         printf("No user is logged in.\n");
//         return;
//     }
//     showProfile(*u);
// }

// void showLeaderboard() {
//     User *list = NULL;
//     int count = 0;

//     loadUsers(&list, &count);

//     if (count == 0) {
//         printf("No users to show.\n");
//         return;
//     }

//     // simple bubble sort by reward (descending)
//     for (int i = 0; i < count; i++) {
//         for (int j = i + 1; j < count; j++) {
//             if (list[j].reward > list[i].reward) {
//                 User tmp = list[i];
//                 list[i] = list[j];
//                 list[j] = tmp;
//             }
//         }
//     }

//     printf("\n===== LEADERBOARD =====\n");
//     for (int i = 0; i < count; i++) {
//         printf("%d. %s - %d\n", i + 1, list[i].name, list[i].reward);
//     }
//     printf("=======================\n");

//     free(list);
// }

// int main() {
//     User *users = NULL;
//     int totalUsers = 0;
//     User *current = NULL;

//     while (1) {
//         int ch;
//         printf("\n========== MENU ==========\n");
//         printf("1. Create User\n");
//         printf("2. Login / Switch User\n");
//         printf("3. View Current User\n");
//         printf("4. Reward Leaderboard\n");
//         printf("5. Exit\n");
//         printf("Enter choice: ");
//         scanf("%d",&ch);

//         switch(ch){
//             case 1:
//                 addUser();
//                 break;

//             case 2:
//                 loadUsers(&users, &totalUsers);
//                 if (totalUsers == 0) {
//                     printf("No users found.\n");
//                     break;
//                 }
                
//                 {
//                     int idx = login(users, totalUsers);
//                     if (idx != -1) {
//                         current = &users[idx];
//                         printf("Now active: %s\n", current->name);
//                     }
//                 }
//                 break;

//             case 3:
//                 showCurrentUser(current);
//                 break;

//             case 4:
//                 showLeaderboard();
//                 break;

//             case 5:
//                 printf("Goodbye!\n");
//                 free(users);
//                 return 0;

//             default:
//                 printf("Invalid choice.\n");
//                 break;
//         }
//     }
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sha256.h"

#define FILE_NAME "users.txt"

typedef struct {
    char name[32];
    char password[65];
    int DOB;
    int reward;
} User;

// CHECK IF USERNAME ALREADY EXISTS
int usernameExists(const char *name) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0;

    char uname[32], pwd[65];
    int r;

    while (fscanf(fp, "%s %s %d", uname, pwd, &r) == 3) {
        if (strcmp(uname, name) == 0) {
            fclose(fp);
            return 1;  // found
        }
    }

    fclose(fp);
    return 0;
}

void hash_password(const char* password, char hex_output[65]) {
    BYTE hash_raw[SHA256_BLOCK_SIZE];
    SHA256_CTX ctx;
    
    // Hash the password
    sha256_init(&ctx);
    sha256_update(&ctx, (const BYTE*)password, strlen(password));
    sha256_final(&ctx, hash_raw);

    // Convert the raw hash to a hexadecimal string
    for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
        sprintf(hex_output + (i * 2), "%02x", hash_raw[i]);
    }
    hex_output[64] = '\0';
}


// CREATE USER
void addUser() {
    FILE *fp = fopen(FILE_NAME, "a");
    if (!fp) {
        printf("Cannot open file.\n");
        return;
    }

    User u;

    // ask until unique username given
    while (1) {
        printf("Enter username: ");
        scanf("%31s", u.name);

        if (usernameExists(u.name)) {
            printf("Username already exists. Choose a different name.\n");
        } else {
            break;
        }
    }

    printf("Enter password: ");
    scanf("%49s", u.password);
    char hashed_pwd[65];
    hash_password(u.password, hashed_pwd);

    printf("Enter Date of Birth for security purposes in DDMMYY format: ");
    scanf("%d",&u.DOB);

    u.reward = 0;

    fprintf(fp, "%s %s %d %d\n", u.name, hashed_pwd, u.reward,u.DOB);
    fclose(fp);

    printf("User created.\n");
}
// LOAD USERS FROM FILE
void loadUsers(User **list, int *count) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) {
        *count = 0;
        return;
    }

    int c = 0;
    char a[32], b[65];
    int r,d;

    while (fscanf(fp, "%s %s %d %d", a, b, &r,&d) == 4)
        c++;

    rewind(fp);  //kind of seek() function in python

    *list = malloc(sizeof(User) * c);
    *count = c;

    for (int i = 0; i < c; i++) {
        fscanf(fp, "%s %s %d %d",(*list)[i].name,(*list)[i].password,&(*list)[i].reward,&(*list)[i].DOB);
    }

    fclose(fp);
}
// LOGIN FUNCTION

int login(char* username_out) {
    char input_user[32], input_pass[50],input_pass_hash[65];
    int attempts = 0;

    while (attempts < 3) {
        printf("Username: ");
        scanf("%31s", input_user);
        printf("Password: ");
        scanf("%49s", input_pass);

        hash_password(input_pass, input_pass_hash);

        FILE* fp = fopen("users.txt", "r");
        if (fp == NULL) {
            printf("User database missing.\n");
            return 0;
        }

        char file_user[32], file_pass[65];
        int success = 0,reward;
        while (fscanf(fp, "%s %s %d %d", file_user, file_pass,&reward) == 3) {
            if (strcmp(input_user, file_user) == 0 &&
                strcmp(input_pass_hash, file_pass) == 0) {
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






// SHOW PROFILE OF A USER

void showProfile(User u) {
    printf("\n--- PROFILE ---\n");
    printf("Name: %s\n", u.name);
    printf("DOB(DD/MM/YYYY): %d\n", u.DOB);

    printf("Reward: %d\n", u.reward);
    printf("---------------\n");
}


// SHOW CURRENT USER

void showCurrentUser(User *u) {
    if (u == NULL) {
        printf("No user is logged in.\n");
        return;
    }
    showProfile(*u);
}

// LEADERBOARD

void showLeaderboard() {
    User *list = NULL;
    int count = 0;

    loadUsers(&list, &count);

    if (count == 0) {
        printf("No users to show.\n");
        return;
    }

    // sort by reward (descending)
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            if (list[j].reward > list[i].reward) {
                User tmp = list[i];
                list[i] = list[j];
                list[j] = tmp;
            }
        }
    }

    printf("\n===== LEADERBOARD =====\n");
    for (int i = 0; i < count; i++) {
        printf("%d. %s - %d\n", i + 1, list[i].name, list[i].reward);
    }
    printf("=======================\n");

    free(list);
}

// In user.c

void update_reward_score(const char* username, int points_to_add) {
    // Load all users from users.txt into an array
    User* users = NULL;
    int count = 0;
    loadUsers(&users, &count);

    int user_index = -1;
    //find the current user 
    for (int i = 0; i < count; i++) {
        if (strcmp(users[i].name, username) == 0) {
            user_index = i;
            break;
        }
    }

    if (user_index == -1) {
        free(users);
        return; // Should not happen if user is logged in
    }


    users[user_index].reward += points_to_add;

    FILE* fp = fopen(FILE_NAME, "w"); // "w" to overwrite
    if (!fp) { /* handle error */ return; }
    
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s %s %d\n", users[i].name, users[i].password, users[i].reward);
    }
    
    fclose(fp);
    free(users);
}

/*int main() {
    User *users = NULL;
    int totalUsers = 0;
    User *current = NULL;
    printf("\n========== Welcome to our task manager ==========\n");

    while (1) {
        int ch;
        printf("\n========== MENU ==========\n");
        printf("1. Create User\n");
        printf("2. Login / Switch User\n");
        printf("3. View Current User\n");
        printf("4. Reward Leaderboard\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &ch);

        switch (ch) {
            case 1:
                addUser();
                break;

            case 2:
                loadUsers(&users, &totalUsers);
                if (totalUsers == 0) {
                    printf("No users found.\n");
                    break;
                }
                {
                    int idx = login(users, totalUsers);
                    if (idx != -1) {
                        current = &users[idx];
                        printf("Now active: %s\n", current->name);
                    }
                }
                break;

            case 3:
                showCurrentUser(current);
                break;

            case 4:
                showLeaderboard();
                break;

            case 5:
                printf("Goodbye!\n");
                free(users);
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }

}*/


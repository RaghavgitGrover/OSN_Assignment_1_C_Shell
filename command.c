#include "headers.h"
#include <linux/time.h>

extern const char *valid_commands_system[];
extern const char *valid_commands_own[];
extern Alias *aliases;
extern int ctrlc_flag, ctrlz_flag;
extern int flag_greater_than_2;
extern int time_greater_than_2;
extern char command_greater_than_2[MAX_LEN];

int is_valid_command(char *command) {
    for (int i = 0; valid_commands_system[i] != NULL; i++) if (strcmp(command, valid_commands_system[i]) == 0) return 1;
    for (int i = 0; valid_commands_own[i] != NULL; i++) if (strcmp(command, valid_commands_own[i]) == 0) return 2;
    Alias *temp = aliases;
    while (temp != NULL) {
        if (strcmp(command, temp->alias_name) == 0) return 3;
        temp = temp->next;
    }
    return 0; 
}

void execute_command(char **args, int background, int nature_of_command) {
    // print the command
    // printf("Executing command: ");
    // for (int i = 0; args[i] != NULL; i++) printf("%s ", args[i]);
    // printf("\n");

    if (nature_of_command == 1) {
        int pid = fork(); 
        if (pid < 0) {
            printf("Fork failed\n");
            return;
        }
        if (pid == 0) { 
            if (background) signal(SIGINT, SIG_IGN);
            if (strcmp(args[0], "vim") == 0 && background) exit(0); 
            execvp(args[0], args);
            printf("Exec failed\n");
            return;
        } 
        else { 
            if (background) {
                dprintf(STDERR_FILENO, "[%d] %d\n", getpid(), pid);  
                fflush(stdout);
                add_background_process(pid, args[0]);
            } 
            else {
                add_foreground_process(pid, args[0]);
                struct timespec start_time, end_time;
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                int status;
                waitpid(pid, &status, 0);
                clock_gettime(CLOCK_MONOTONIC, &end_time);
                int time_taken = end_time.tv_sec - start_time.tv_sec;
                // if (time_taken > 2) printf("%s took %ds\n", args[0], time_taken);
                if (time_taken > 2) {
                    flag_greater_than_2 = 1;
                    time_greater_than_2 = time_taken;
                    strcpy(command_greater_than_2, args[0]);
                }
                if (WIFEXITED(status) && !ctrlc_flag) printf("%s exited normally with status %d\n", args[0], WEXITSTATUS(status));
                // else printf("%s did not exit normally\n", args[0]);
            }
        }
    }
    else if (nature_of_command == 2) {
        if (strcmp(args[0], "hop") == 0) hop_command(&args[1]); 
        else if (strcmp(args[0], "reveal") == 0) reveal_command(&args[1]);
        else if (strcmp(args[0], "proclore") == 0) proclore_command(&args[1]);
        else if (strcmp(args[0], "seek") == 0) seek_command(&args[1]);          
        else if (strcmp(args[0], "log") == 0) log_command(&args[1]);
        else if (strcmp(args[0], "iMan") == 0) iMan_command(&args[1]);
        else if (strcmp(args[0], "activities") == 0) activities_command();
        else if (strcmp(args[0], "ping") == 0) ping_command(&args[1]);
        else if (strcmp(args[0], "fg") == 0) fg_command(&args[1]);
        else if (strcmp(args[0], "bg") == 0) bg_command(&args[1]);
        else if (strcmp(args[0], "neonate") == 0) neonate_command(&args[1]);
        else if (strcmp(args[0], "exit") == 0) {
            terminate_all_background_processes();
            exit(0);
        }
    }
    else if (nature_of_command == 3) {
        Alias *temp = aliases;
        while (temp != NULL) {
            if (strcmp(args[0], temp->alias_name) == 0) {
                char *alias_command = (char *)malloc(strlen(temp->command) + 1);
                strcpy(alias_command, temp->command);
                char *saveptr = NULL;
                char *token = strtok_r(alias_command, " \t", &saveptr);
                int argc = 0;
                char *new_args[MAX_LEN];
                while (token != NULL) {
                    new_args[argc++] = token;
                    token = strtok_r(NULL, " \t", &saveptr);
                }
                new_args[argc] = NULL;
                int nature_of_command = is_valid_command(new_args[0]);
                execute_command(new_args, background, nature_of_command);
                return;
            }
            temp = temp->next;
        }
    }
    else printf("%s is not a valid command\n", args[0]);
    return;
}
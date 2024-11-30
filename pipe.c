#include "headers.h"
extern char *valid_commands_system[];

void handle_pipe_command(char *input) {
    int num_commands = 0;
    char *commands[MAX_LEN];
    char *command = strtok(input, "|");
    while (command != NULL) {
        command = trim_whitespace(command);
        if (strlen(command) == 0) { 
            printf("Invalid use of pipe\n");
            return;
        }
        commands[num_commands++] = command;
        command = strtok(NULL, "|");
    }
    if (num_commands < 2) {
        printf("Invalid use of pipe\n");
        return;
    }

    int pipefds[2 * (num_commands - 1)];
    for (int i = 0; i < (num_commands - 1); i++) {
        if (pipe(pipefds + i*2) < 0) {
            printf("Couldn't create pipe\n");
            exit(0);
        }
    }

    for (int i = 0; i < num_commands; i++) {
        int pid = fork();
        if (pid == 0) {
            if ((i != 0 && (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) < 0)) || ((i != num_commands - 1) && dup2(pipefds[i * 2 + 1], STDOUT_FILENO) < 0)) {
                printf("Couldn't execute dup2\n");
                exit(0);
            }
            for (int j = 0; j < 2 * (num_commands - 1); j++) close(pipefds[j]);
            char *args[MAX_LEN];
            int argc = 0;
            char *arg = strtok(commands[i], " \t");
            while (arg != NULL) {
                args[argc++] = arg;
                arg = strtok(NULL, " \t");
            }
            args[argc] = NULL;
            // int is_system_command = 0;
            // for (int j = 0; valid_commands_system[j] != NULL; j++) {
            //     if (strcmp(args[0], valid_commands_system[j]) == 0) {
            //         is_system_command = 1;
            //         break;
            //     }
            // }
            // if (is_system_command) {
            //     for (int j = 0; j < argc; j++) printf("%s\n", args[j]);
            //     if (execvp(args[0], args) < 0) {
            //         printf("Couldn't execute execvp\n");
            //         exit(0);
            //     }
            // } 
            // else {
                char full_command[MAX_LEN] = "";
                strcat(full_command, args[0]);
                for (int j = 1; j < argc && args[j] != NULL; j++) {
                    strcat(full_command, " ");
                    strcat(full_command, args[j]);
                }
                // printf("full_command: %s\n", full_command);
                // fflush(stdout);
                handle_individual_command(full_command);
                exit(0);
            // }
        } 
        else if (pid < 0) {
            printf("Couldn't execute fork\n");
            exit(0);
        }
    }

    for (int i = 0; i < 2 * (num_commands - 1); i++) close(pipefds[i]);
    for (int i = 0; i < num_commands; i++) wait(NULL);
}

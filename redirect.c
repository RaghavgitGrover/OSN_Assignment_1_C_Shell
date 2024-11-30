#include "headers.h"
extern int append_mode;
extern char *input_file;
extern char *output_file;
extern const char *valid_commands_system[];

void handle_redirect_command(char *command) {
    char *processed_command1 = remove_spaces(command);
    if (command[0] == '<' || command[0] == '>' || command[strlen(command) - 1] == '<' || command[strlen(command) - 1] == '>') {
        printf("Invalid use of redirection\n");
        return;
    }
    for (int i = 0; i < strlen(command) - 1; i++) {
        if ((command[i] == '<' && command[i + 1] == '>') || (command[i] == '>' && command[i + 1] == '<')) {
            printf("Invalid use of redirection\n");
            return;
        }
    }
    char processed_command2[MAX_LEN];
    char *temp_command = add_spaces(command, processed_command2);
    strcpy(command, temp_command);
    // printf("Command: %s\n", command);
    input_file = NULL;
    output_file = NULL;
    append_mode = 0; 
    int argc = 0;
    char *args[MAX_LEN];
    char *arg = strtok(command, " \t");
    while (arg != NULL) {
        args[argc++] = arg;
        arg = strtok(NULL, " \t");
    }
    args[argc] = NULL;
    if (argc <= 0) {
        printf("ERROR");
        return;
    }

    // for (int i = 0; i != argc; i++) printf("%s ", args[i]);
    // printf("\n");

    int c1 = 0, c2 = 0;
    for (int i = 0; i < argc; i++) {
        if (strcmp(args[i], "<") == 0) {
            c1++;
            if (i != argc - 1) {
                input_file = args[i + 1];
                args[i] = NULL; 
                i++;
            } 
            else {
                printf("ERROR: No input file specified\n");
                return;
            }
        } 
        if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0) {
            c2++;
            if (strcmp(args[i], ">>") == 0) append_mode = 1;
            if (i != argc - 1) {
                output_file = args[i + 1];
                args[i] = NULL; 
                i++;
            } 
            else {
                printf("ERROR: No output file specified\n");
                return;
            }
        }
    }

    if (c1 > 1 || c2 > 1) {
        printf("ERROR: Multiple redirections not allowed\n");
        return;
    }

    int pid = fork();
    if (pid == -1) {
        printf("Couldn't execute fork\n");
        return;
    }
    if (pid == 0) {
        if (input_file) {
            int fd_in = open(input_file, O_RDONLY);
            if (fd_in == -1) {
                printf("Couldn't open input file\n");
                exit(0);
            }
            if (dup2(fd_in, STDIN_FILENO) == -1) {
                printf("Couldn't execute dup2 for input file\n");
                close(fd_in);
                exit(0);
            }
            close(fd_in);
        }

        if (output_file) {
            int fd_out;
            if (append_mode) fd_out = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            else fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out == -1) {
                printf("Couldn't open output file\n");
                exit(0);
            }
            if (dup2(fd_out, STDOUT_FILENO) == -1) {
                printf("Couldnt execute dup2 for output file\n");
                close(fd_out);
                exit(0);
            }
            close(fd_out);
        }
        int is_system_command = 0;
        for (int i = 0; valid_commands_system[i] != NULL; i++) {
            if (strcmp(args[0], valid_commands_system[i]) == 0) {
                is_system_command = 1;
                break;
            }
        }
        if (is_system_command) {
            execvp(args[0], args);
            printf("Couldn't execute execvp\n");
            exit(0);
        }
        else {
            char full_command[MAX_LEN] = "";
            strcat(full_command, args[0]);
            for (int i = 1; i < argc && args[i] != NULL; i++) {
                strcat(full_command, " ");
                strcat(full_command, args[i]);
            }
            handle_individual_command(full_command);
            exit(0);
        }
    } 
    else { 
        int status;
        waitpid(pid, &status, WUNTRACED);
        if (WIFSIGNALED(status)) printf("Process terminated by signal %d\n", WTERMSIG(status));
    }
}
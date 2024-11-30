#include "headers.h"
extern int append_mode;
extern char *valid_commands_system[];
extern char *input_file;
extern char *output_file;


void handle_pipdirect_command(char *input) {
    char *processed_input = remove_spaces(input);
    // printf("processed_input: %s\n", processed_input);
    int flag = 0;
    for (int i = 0; processed_input[i] != '\0'; i++) {
        if (processed_input[i] == '|') {
            if (i > 0 && (processed_input[i - 1] == '<' || processed_input[i - 1] == '>')) {
                flag = 1;
                break;
            }
            if (processed_input[i + 1] != '\0' && (processed_input[i + 1] == '<' || processed_input[i + 1] == '>')) {
                flag = 1;
                break;
            }
        }
    }
    if (processed_input[0] == '|' || processed_input[0] == '<' || processed_input[0] == '>') flag = 1;
    if (processed_input[strlen(processed_input) - 1] == '|' || processed_input[strlen(processed_input) - 1] == '<' || processed_input[strlen(processed_input) - 1] == '>') flag = 1;
    if (flag) {
        printf("Invalid use of pipe\n");
        return;
    }

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

    int pipefds[2 * (MAX_LEN - 1)]; 
    for (int i = 0; i < (num_commands - 1); i++) {
        if (pipe(pipefds + i*2) < 0) {
            printf("Couldn't create pipe\n");
            exit(0);
        }
    }

    for (int j = 0; j < num_commands; j++) {
        int pid = fork();
        if (pid == 0) { 
            if (j > 0) {
                if (dup2(pipefds[(j - 1) * 2], STDIN_FILENO) < 0) {
                    printf("dup2 for input pipe");
                    exit(0);
                }
            }
            if (j < num_commands - 1) {
                if (dup2(pipefds[j * 2 + 1], STDOUT_FILENO) < 0) {
                    printf("dup2 for output pipe");
                    exit(0);
                }
            }

            for (int i = 0; i < 2 * (num_commands - 1); i++) close(pipefds[i]);
            char *args[MAX_LEN];
            int argc = 0;
            char *arg = strtok(commands[j], " \t");
            while (arg != NULL) {
                args[argc++] = arg;
                arg = strtok(NULL, " \t");
            }
            args[argc] = NULL;

            input_file = NULL;
            output_file = NULL;
            append_mode = 0; 
            for (int i = 0; i < argc; i++) {
                if (strcmp(args[i], "<") == 0) {
                    if (i + 1 < argc) {
                        input_file = args[i + 1];
                        args[i] = NULL; 
                        i++;
                    } 
                    else {
                        printf("ERROR: No input file specified\n");
                        exit(0);
                    }
                } 
                else if (strcmp(args[i], ">") == 0) {
                    if (i + 1 < argc) {
                        output_file = args[i + 1];
                        append_mode = 0; 
                        args[i] = NULL; 
                        i++;
                    } 
                    else {
                        printf("ERROR: No output file specified\n");
                        exit(0);
                    }
                } 
                else if (strcmp(args[i], ">>") == 0) {
                    if (i + 1 < argc) {
                        output_file = args[i + 1];
                        append_mode = 1; 
                        args[i] = NULL;
                        i++;
                    } else {
                        printf("ERROR: No output file specified\n");
                        exit(0);
                    }
                }
            }

            if (input_file) {
                int fd_in = open(input_file, O_RDONLY);
                if (fd_in == -1) {
                    printf("open input file failed");
                    exit(0);
                }
                if (dup2(fd_in, STDIN_FILENO) == -1) {
                    printf("dup2 for input file failed");
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
                    printf("open output file failed");
                    exit(0);
                }
                if (dup2(fd_out, STDOUT_FILENO) == -1) {
                    printf("dup2 for output file failed");
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
            if (is_system_command) execvp(args[0], args);
            else handle_individual_command(commands[j]);
            printf("exec failed");
            exit(0);

            // handle_redirect_command(commands[j]);
            // exit(0);
        } 
        else if (pid < 0) {
            printf("fork");
            exit(0);
        }
    }

    for (int i = 0; i < 2 * (num_commands - 1); i++) close(pipefds[i]);
    for (int i = 0; i < num_commands; i++) wait(NULL);
}



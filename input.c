#include "headers.h"
extern int log_count;
void process_input(char *input) {
    int len = strlen(input);
    if (len != 0 && input[len - 1] == '\n') input[len - 1] = '\0';
    char *saveptr = NULL; 
    char *token = strtok_r(input, ";", &saveptr);
    while (token != NULL) {
        if (strchr(token, '&') != NULL) {
            char *temp = (char *)malloc(MAX_LEN);
            strcpy(temp, "");
            int j = 0;
            for (int i = 0; i < strlen(token); i++) {
                if (token[i] == '&') {
                    temp[j++] = '&';
                    temp[j] = '\0';
                    handle_individual_command(temp);
                    j = 0;
                    strcpy(temp, "");
                }
                else {
                    temp[j++] = token[i];
                }
            }
            if (j > 0) {
                temp[j] = '\0';
                handle_individual_command(temp);
            }
            free(temp);
        }
        else handle_individual_command(token);
        token = strtok_r(NULL, ";", &saveptr);
    }
    return;
}

void handle_individual_command(char *command) {
    if ((strchr(command, '<') != NULL || strchr(command, '>') != NULL || strchr(command, ">>")) && strchr(command, '|') != NULL) {
        handle_pipdirect_command(command);
        return;
    }
    if (strchr(command, '<') != NULL || strchr(command, '>') != NULL || strchr(command, ">>")) {
        handle_redirect_command(command);
        return;
    }
    if (strchr(command, '|') != NULL) {
        handle_pipe_command(command);
        return;
    }    

    command = trim_whitespace(command);
    int background = 0, argc = 0;
    char *ampersand = strchr(command, '&');
    if (ampersand != NULL) {
        *ampersand = '\0';  
        background = 1;
    }
    
    char *args[MAX_LEN];
    char *arg = strtok(command, " \t");
    while (arg != NULL) {
        args[argc++] = arg;
        arg = strtok(NULL, " \t");
    }
    args[argc] = NULL;  
    if (argc <= 0) return;
    int nature_of_command = is_valid_command(args[0]);
    // if (nature_of_command == 0) printf("ERROR : %s is not a valid command\n", args[0]);
    execute_command(args, background, nature_of_command);
    return;
}

char* trim_whitespace(char *str) {
    char *start = str, *end = str + strlen(str) - 1;
    for (; *start == ' ' || *start == '\t'; start++);
    for (; end > start && (*end == ' ' || *end == '\t'); end--);
    *(end + 1) = '\0';
    return start;
}

#include "headers.h" 

extern char log_buffer[MAX_LOGS][MAX_LEN];
extern int log_index;
extern int log_count;
extern char home_dir[MAX_LEN];
extern log_file_path[MAX_LEN];

void load_log() {
    FILE *file = fopen(log_file_path, "r");
    if (file == NULL) {
        printf("Could not open log file");
        return;
    }
    while (fgets(log_buffer[log_index], sizeof(log_buffer[log_index]), file) != NULL) {
        char *newline_pos = strchr(log_buffer[log_index], '\n');
        if (newline_pos) *newline_pos = '\0';
        log_index = (log_index + 1) % MAX_LOGS;
        if (log_count < MAX_LOGS) log_count++;
    }
    fclose(file);
    return;
}

void save_log() {
    FILE *file = fopen(log_file_path, "w");
    if (file == NULL) {
        printf("Could not open log file");
        return;
    }
    int start_index = (log_index - log_count + MAX_LOGS) % MAX_LOGS;
    for (int i = 0; i < log_count; i++) fprintf(file, "%s\n", log_buffer[(start_index + i) % MAX_LOGS]);
    fflush(file);  
    fclose(file);
    return;
}

void add_to_log(char *command) {
    int len = strlen(command);
    if (len > 0 && command[len - 1] == '\n') command[len - 1] = '\0';
    if (log_count > 0 && strcmp(command, log_buffer[(log_index - 1 + MAX_LOGS) % MAX_LOGS]) == 0) return;
    strcpy(log_buffer[log_index], command);
    log_buffer[log_index][sizeof(log_buffer[log_index]) - 1] = '\0';
    log_index = (log_index + 1) % MAX_LOGS;
    if (log_count < MAX_LOGS) log_count++;
    save_log();
    return;
}

void log_command(char **args) {
    if (args[0] == NULL) {
        if (log_count == 0) {
            printf("Log is empty\n");
            return;
        }
        printf("Log commands oldest to newest\n");
        int start_index = (log_index - log_count + MAX_LOGS) % MAX_LOGS;
        for (int i = 0; i < log_count; i++) printf("[%d] %s\n", log_count - i, log_buffer[(start_index + i) % MAX_LOGS]);
    } 
    else if (strcmp(args[0], "purge") == 0) {
        if (remove(log_file_path) != 0) printf("Could not delete logs file.\n");
        FILE *file = fopen(log_file_path, "w");
        if (file == NULL) {
            printf("Could not create log file.\n");
            return;
        }
        fclose(file);
        log_count = 0;
        log_index = 0;
    } 
    else if (strcmp(args[0], "execute") == 0 && args[1] != NULL) {
        int index = atoi(args[1]) - 1;
        index = log_count - index - 1;
        if (index >= 0 && index < log_count) {
            char command_to_execute[MAX_LEN];
            strncpy(command_to_execute, log_buffer[(log_index - log_count + index + MAX_LOGS) % MAX_LOGS], sizeof(command_to_execute) - 1);
            command_to_execute[sizeof(command_to_execute) - 1] = '\0';
            if (index != 0) add_to_log(command_to_execute);
            process_input(command_to_execute);
        } 
        else printf("Invalid log index\n");
    } 
    else printf("Invalid log command\n");
    return;
}

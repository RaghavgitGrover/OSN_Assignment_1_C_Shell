#include "headers.h"

extern int is_file_only;
extern int is_dir_only;
extern int execute_flag;
extern int match_count;
extern char cwd[MAX_LEN];
extern char prev_dir[MAX_LEN];
extern char home_dir[MAX_LEN];

void print_path(char *full_path, char *color) {
    const char *last_slash = strrchr(full_path, '/');
    int length = strlen(full_path);
    for (int i = length - 1; i >= 0; i--) {
        if (full_path[i] == '/') {
            // printf(".%s\n", full_path + i);
            printf(color);
            printf(".%s", full_path + i);
            printf(RESET "\n");
        }   
    }
    return;
}

void print_file_content(const char *path) {
    FILE *file = fopen(path, "r");
    if (file) {
        char buffer[MAX_LEN];
        while (fgets(buffer, sizeof(buffer), file)) printf("%s", buffer);
        fclose(file);
    } 
    else printf("Missing permissions for task!\n");
    return;
}

void search_directory(const char *target, const char *directory) {
    DIR *dir;
    if ((dir = opendir(directory)) == NULL) {
        printf("Error opening directory '%s'\n", directory);
        return;
    }
    struct dirent *entry;
    char path[MAX_LEN];
    struct stat statbuf;   
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);
        if (stat(path, &statbuf) == -1) continue;
        int cond1 = is_file_only && S_ISREG(statbuf.st_mode), cond2 = is_dir_only && S_ISDIR(statbuf.st_mode), cond3 = !is_file_only && !is_dir_only;
        int is_match = strstr(entry->d_name, target) != NULL;
        if (is_match && (cond1 || cond2 || cond3)) {
            match_count++;
            if (execute_flag && match_count == 1) {
                if (is_file_only) {
                    printf(GREEN "%s" RESET "\n", path);
                    print_file_content(path);
                } 
                else if (is_dir_only) {                                                           
                    printf(BLUE "%s/" RESET "\n", path);
                    strcpy(prev_dir, cwd);
                    chdir(path);
                    strcpy(cwd, path);
                }
                closedir(dir);
                return;
            } 
            // if (S_ISDIR(statbuf.st_mode)) printf(BLUE);  
            // else printf(GREEN); 
            char color[MAX_LEN];
            if (S_ISDIR(statbuf.st_mode)) strcpy(color, BLUE);
            else strcpy(color, GREEN);
            print_path(path, color);
        }
        if (S_ISDIR(statbuf.st_mode)) search_directory(target, path);
    }
    closedir(dir);
    return;
}

void seek_command(char **args) {
    is_file_only = 0, is_dir_only = 0, execute_flag = 0;
    char *target, *directory = ".";

    int arg_index = 0;
    while (args[arg_index] && args[arg_index][0] == '-') {
        if (strcmp(args[arg_index], "-d") == 0) is_dir_only = 1;
        else if (strcmp(args[arg_index], "-f") == 0) is_file_only = 1;
        else if (strcmp(args[arg_index], "-e") == 0) execute_flag = 1;
        else { 
            printf("Invalid flag: %s\n", args[arg_index]);
            return; 
        }
        arg_index++;
    }
    if (is_dir_only && is_file_only) {
        printf("Cannot specify both -d and -f flags in seek!\n");
        return;
    }
    if (!args[arg_index]) { 
        printf("Target not specified!\n");
        return; 
    }
    target = args[arg_index++];
    if (args[arg_index]) directory = args[arg_index];

    struct stat statbuf;


    if (strcmp(directory, "~") == 0) strcpy(directory, home_dir);  
    else if (strcmp(directory, "-") == 0) strcpy(directory, prev_dir);
    else if (strcmp(directory, "..") == 0) {
        char *last_slash = strrchr(cwd, '/');
        if (last_slash == NULL || last_slash == cwd) strcpy(directory, "/");
        else {
            *last_slash = '\0';
            strcpy(directory, cwd);
        }
    }

    if (stat(directory, &statbuf) != 0 || !S_ISDIR(statbuf.st_mode)) {
        printf("Invalid directory: %s\n", directory);
        return;
    }

    char *extension = strrchr(target, '.');
    if (extension) *extension = '\0';

    match_count = 0;
    search_directory(target, directory);
    if (match_count == 0) printf("No match found!\n");
    else if (execute_flag && match_count > 1) printf("Multiple matches found, flag -e has no effect!\n");
    return;
}

#include "headers.h"

extern char home_dir[MAX_LEN];
extern char cwd[MAX_LEN];
extern char prev_dir[MAX_LEN];
extern char temp_dir[MAX_LEN];

void change_directory(const char *path) {
    if (chdir(path) == -1) {
        if (strcmp(path, "") == 0) printf("Already reached the topmost directory\n");
        else printf("Could not change directory to %s\n", path);
        return;
    }
    strcpy(cwd, path);
    strcpy(prev_dir, temp_dir);
    printf("%s\n", cwd); 
    return;
}

void hop_command(char **args) {
    if (args[0] == NULL) {
        change_directory(home_dir);
        strcpy(prev_dir, temp_dir);
        return;
    }
    for (int i = 0; args[i] != NULL; i++) {
        char temp_path[MAX_LEN];
        strcpy(temp_dir, cwd);
        if (strcmp(args[i], ".") == 0) change_directory(cwd);
        else if (strcmp(args[i], "~") == 0) change_directory(home_dir);
        else if (strcmp(args[i], "-") == 0) change_directory(prev_dir);  
        else if (args[i][0] == '/')  {
            change_directory(args[i]);
        }        
        else if (strncmp(args[i], "~/", 2) == 0) {
            snprintf(temp_path, sizeof(temp_path), "%s/%s", home_dir, args[i] + 2);
            change_directory(temp_path);
        }     
        else if (strncmp(args[i], "./", 2) == 0) {
            snprintf(temp_path, sizeof(temp_path), "%s/%s", cwd, args[i] + 2);
            change_directory(temp_path);
        }
        else if (strcmp(args[i], "..") == 0) {
            if (strcmp(cwd, "/") == 0) {
                printf("Already reached the topmost directory\n");
                continue;
            }
            else {
                char *last_slash = strrchr(cwd, '/');
                if (last_slash == NULL || last_slash == cwd) change_directory("/");
                else {
                    *last_slash = '\0';
                    change_directory(cwd);
                }
            } 
            // change_directory(cwd);
        }    
        else if (strncmp(args[i], "../", 3) == 0) {
            snprintf(temp_path, sizeof(temp_path), "%s/%s", cwd, args[i] + 3);
            change_directory(temp_path);
        }
        else {
            snprintf(temp_path, sizeof(temp_path), "%s/%s", cwd, args[i]);
            change_directory(temp_path);
        }
    }
    return;
}

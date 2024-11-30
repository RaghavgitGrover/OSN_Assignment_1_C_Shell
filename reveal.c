#include "headers.h"
#include <time.h>

extern char home_dir[MAX_LEN];
extern char cwd[MAX_LEN];
extern char prev_dir[MAX_LEN];
extern int show_hidden;
extern int show_details;

int compare(const void *a, const void *b) {
    struct dirent *entryA = *(struct dirent **)a;
    struct dirent *entryB = *(struct dirent **)b;
    return strcmp(entryA->d_name, entryB->d_name);
}

void reveal_command(char **args) {
    show_hidden = 0, show_details = 0;
    char *path = cwd;
    for (int i = 0; args[i] != NULL; i++) {
        if (args[i][0] == '-' && args[i][1] != '\0') {
            if (strchr(args[i], 'a')) show_hidden = 1;
            if (strchr(args[i], 'l')) show_details = 1;
        } 
        else if (strcmp(args[i], "-") == 0) strcpy(path, prev_dir);
        else if (strcmp(args[i], "~") == 0) strcpy(path, home_dir);
        else if (strcmp(args[i], ".") == 0) strcpy(path, cwd);
        else if (args[i][0] == '/') {
            strcpy(path, args[i]);
        }
        else if (strncmp(args[i], "~/", 2) == 0) {
            char *temp_path = (char *)malloc(MAX_LEN * sizeof(char));
            snprintf(temp_path, MAX_LEN, "%s/%s", home_dir, args[i] + 2);
            strcpy(path, temp_path);
        }
        else if (strncmp(args[i], "./", 2) == 0) {
            char *temp_path = (char *)malloc(MAX_LEN * sizeof(char));
            snprintf(temp_path, MAX_LEN, "%s/%s", cwd, args[i] + 2);             
            strcpy(path, temp_path);
        }
        else if (strncmp(args[i], "../", 3) == 0) {
            char *temp_path = (char *)malloc(MAX_LEN * sizeof(char));
            snprintf(temp_path, MAX_LEN, "%s/%s", cwd, args[i] + 3);
            strcpy(path, temp_path);
        }
        else if (strcmp(args[i], "..") == 0) {
            if (strcmp(path, "/") == 0) {
                printf("Already reached the topmost directory\n");
                continue;
            }
            else {
                char *last_slash = strrchr(path, '/');
                if (last_slash == NULL || last_slash == path) strcpy(path, "/");
                else {
                    *last_slash = '\0';
                    strcpy(path, path);
                }
            }
        }
        else {
            char *temp_path = (char *)malloc(MAX_LEN * sizeof(char));
            snprintf(temp_path, MAX_LEN, "%s/%s", path, args[i]);
            strcpy(path, temp_path);
        }
    }

    DIR *dir = opendir(path);
    if (dir == NULL) {
        printf("Error opening directory\n");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!show_hidden && entry->d_name[0] == '.') continue;
        if (show_details) {
            char full_path[MAX_LEN];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
            struct stat fileStat;
            if (stat(full_path, &fileStat) == -1) {
                printf("Error getting file stats\n");
                continue;
            }

            if (S_ISDIR(fileStat.st_mode)) printf(BLUE "d");
            else if (fileStat.st_mode & S_IXUSR) printf(GREEN "-");
            else if (S_ISLNK(fileStat.st_mode)) printf(BLUE "l");
            else if (S_ISREG(fileStat.st_mode)) printf(WHITE "-");
            else if (S_ISCHR(fileStat.st_mode)) printf(WHITE "c");
            else if (S_ISBLK(fileStat.st_mode)) printf(WHITE "b");
            else if (S_ISFIFO(fileStat.st_mode)) printf(WHITE "p");
            else if (S_ISSOCK(fileStat.st_mode)) printf(WHITE "s");
            else printf(WHITE "?");

            if (fileStat.st_mode & S_IRUSR) printf("r");
            else printf("-");
            if (fileStat.st_mode & S_IWUSR) printf("w");
            else printf("-");
            if (fileStat.st_mode & S_IXUSR) printf("x");
            else printf("-");
            if (fileStat.st_mode & S_IRGRP) printf("r");
            else printf("-");
            if (fileStat.st_mode & S_IWGRP) printf("w");
            else printf("-");
            if (fileStat.st_mode & S_IXGRP) printf("x");
            else printf("-");
            if (fileStat.st_mode & S_IROTH) printf("r");
            else printf("-");
            if (fileStat.st_mode & S_IWOTH) printf("w");
            else printf("-");
            if (fileStat.st_mode & S_IXOTH) printf("x");
            else printf("-");
            printf(" %lu", fileStat.st_nlink);

            struct passwd *pw = getpwuid(fileStat.st_uid);
            if (pw) printf(" %s", pw->pw_name);
            else printf("Error getting user\n");
            struct group *gr = getgrgid(fileStat.st_gid);
            if (gr) printf(" %s", gr->gr_name);
            else printf("Error getting group\n");

            char time[MAX_LEN];
            struct tm *local_time = localtime(&fileStat.st_mtime);
            if (local_time == NULL) {
                printf("Error getting local time\n");
                continue;
            }
            if (strftime(time, sizeof(time), "%b %d %H:%M", local_time) == 0) {
                printf("Error using strftime\n");
                continue;
            }
            printf(" %ld %s ", fileStat.st_size, time);
            printf("%s" RESET "\n", entry->d_name);
        } 
        else {
            char full_path[MAX_LEN];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
            struct stat fileStat;
            if (stat(full_path, &fileStat) == -1) {
                printf("Error getting file stats\n");
                continue;
            }
            if (S_ISDIR(fileStat.st_mode)) printf(BLUE "%s\n" RESET, entry->d_name);
            else if (fileStat.st_mode & S_IXUSR) printf(GREEN "%s\n" RESET, entry->d_name);
            else printf(WHITE "%s\n" RESET, entry->d_name);
        }
    }
    closedir(dir);
    return;
}











// void reveal_command(char **args) {
//     int show_hidden = 0, show_details = 0;
//     char path[MAX_LEN];
//     strcpy(path, cwd); 
//     for (int i = 0; args[i] != NULL; i++) {
//         if (args[i][0] == '-' && args[i][1] != '\0') {
//             if (strchr(args[i], 'a')) show_hidden = 1;
//             if (strchr(args[i], 'l')) show_details = 1;
//         } 
//         else if (strcmp(args[i], "-") == 0) strcpy(path, prev_dir);
//         else if (strcmp(args[i], "~") == 0) strcpy(path, home_dir);
//         else if (strcmp(args[i], ".") == 0) strcpy(path, cwd);
//         else if (args[i][0] == '/') strcpy(path, args[i]);
//         else if (strncmp(args[i], "~/", 2) == 0) {
//             snprintf(path, MAX_LEN, "%s/%s", home_dir, args[i] + 2);
//         } 
//         else if (strncmp(args[i], "./", 2) == 0) {
//             snprintf(path, MAX_LEN, "%s/%s", cwd, args[i] + 2);
//         } 
//         else if (strncmp(args[i], "../", 3) == 0) {
//             snprintf(path, MAX_LEN, "%s/%s", cwd, args[i] + 3);
//         } 
//         else if (strcmp(args[i], "..") == 0) {
//             if (strcmp(path, "/") == 0) printf("Already at root directory\n"); 
//             else {
//                 char *last_slash = strrchr(path, '/');
//                 if (last_slash == path) strcpy(path, "/");
//                 else *last_slash = '\0';
//             }
//         } 
//         else {
//             snprintf(path, MAX_LEN, "%s/%s", path, args[i]);
//         }
//     }

//     DIR *dir = opendir(path);
//     if (dir == NULL) {
//         printf("Error opening directory: %s\n", path);
//         return;
//     }
//     struct dirent **entries = NULL;
//     int entry_count = 0;
//     struct dirent *entry;
    
//     while ((entry = readdir(dir)) != NULL) {
//         if (!show_hidden && entry->d_name[0] == '.') continue;  
//         entries = realloc(entries, (entry_count + 1) * sizeof(struct dirent *));
//         entries[entry_count] = entry;
//         entry_count++;
//     }
//     closedir(dir);
//     qsort(entries, entry_count, sizeof(struct dirent *), compare);
//     for (int i = 0; i < entry_count; i++) {
//         entry = entries[i];
//         char full_path[MAX_LEN];
//         snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
//         struct stat fileStat;
//         if (stat(full_path, &fileStat) == -1) {
//             printf("Error getting file stats for %s\n", entry->d_name);
//             continue;
//         }
//         // if (show_details) display_file_details(fileStat, entry->d_name);
//         if (show_details) {
//             printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
//             printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
//             printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
//             printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
//             printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
//             printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
//             printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
//             printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
//             printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
//             printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
//             printf(" %lu", fileStat.st_nlink);
//             struct passwd *pw = getpwuid(fileStat.st_uid);
//             printf(" %s", pw ? pw->pw_name : "UNKNOWN");
//             struct group *gr = getgrgid(fileStat.st_gid);
//             printf(" %s", gr ? gr->gr_name : "UNKNOWN");
//             printf(" %ld", fileStat.st_size);
//             char time_buf[20];
//             strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", localtime(&fileStat.st_mtime));
//             printf(" %s", time_buf);
//             printf(" %s\n", entry->d_name);
//         }
//         else {
//             if (S_ISDIR(fileStat.st_mode)) printf(BLUE "%s\n" RESET, entry->d_name);
//             else if (fileStat.st_mode & S_IXUSR) printf(GREEN "%s\n" RESET, entry->d_name);
//             else printf(WHITE "%s\n" RESET, entry->d_name);
//         }
//     }
//     free(entries);  
//     return;
// }

#include "headers.h"

extern char username[MAX_LEN];
extern char hostname[MAX_LEN];
extern char home_dir[MAX_LEN];
extern char cwd[MAX_LEN];
extern int flag_greater_than_2;
extern int time_greater_than_2;
extern char command_greater_than_2[MAX_LEN];

void prompt() {
    getcwd(cwd, MAX_LEN);
    printf("<%s@%s:", username, hostname);
    char *common_str = strstr(cwd, home_dir);
    char *relative_path = common_str + strlen(home_dir); 
    // if (common_str == NULL) printf("%s> ", cwd);
    // else printf("~%s> ", relative_path);

    if (common_str == NULL) printf("%s", cwd);
    else printf("~%s", relative_path);
    if (flag_greater_than_2) {
        printf(" %s : %ds> ", command_greater_than_2, time_greater_than_2);
        flag_greater_than_2 = 0;
    }
    else printf("> ");
    return;
}

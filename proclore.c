#include "headers.h"

extern ProcessStruct *bg_processes;

void proclore_command(char **args) {   
    int pid = getpid();
    if (args[0] != NULL) pid = atoi(args[0]); 
    
    char proc_path[PATH_MAX];
    snprintf(proc_path, sizeof(proc_path), "/proc/%d/stat", pid);
    FILE *stat_file = fopen(proc_path, "r");
    if (stat_file == NULL) {
        printf("Process with PID %d does not exist or has already terminated.\n", pid);
        return;
    }

    char state;
    int pgrp, foreground = 1;
    unsigned long vsize;
    
    fscanf(stat_file, "%*d %*s %c %*d %d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*d %*d %*d %*d %lu", &state, &pgrp, &vsize);
    fclose(stat_file);
    printf("pid : %d\n", pid);
    printf("Process Status : %c", state);

    ProcessStruct *temp = bg_processes;
    while (temp != NULL) {
        if (temp->pid == pid) {
            foreground = 0;
            break;
        }
        temp = temp->next;
    }
    foreground == 1 ? printf("+\n") : printf("\n");

    printf("Process Group : %d\n", pgrp);
    printf("Virtual Memory : %lu KB\n", vsize);

    snprintf(proc_path, sizeof(proc_path), "/proc/%d/exe", pid);
    char exec_path[PATH_MAX];
    int len = readlink(proc_path, exec_path, sizeof(exec_path) - 1);
    if (len != -1) {
        exec_path[len] = '\0';
        printf("Executable path : %s\n", exec_path);
    }
    else printf("Executable path not found\n");
    return;
}
#include "headers.h"
extern ProcessStruct *fg_processes;
extern ProcessStruct *bg_processes;

void add_foreground_process(int pid, char *command) {
    ProcessStruct *new_process = (ProcessStruct *)malloc(sizeof(ProcessStruct));
    new_process->pid = pid;
    strcpy(new_process->command, command);
    new_process->next = fg_processes;
    fg_processes = new_process;
    return;
}

void remove_fg_process(int pid) {
    ProcessStruct *current = fg_processes, *previous = NULL;
    while (current != NULL) {
        if (current->pid == pid) {
            if (previous == NULL) fg_processes = current->next;
            else previous->next = current->next;
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
    return;
}


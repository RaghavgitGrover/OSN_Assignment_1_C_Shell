#include "headers.h"
extern ProcessStruct *bg_processes;
extern int signal_pid, signal_number;

void add_background_process(int pid, char *command) {
    ProcessStruct *new_process = (ProcessStruct *)malloc(sizeof(ProcessStruct));
    new_process->pid = pid;
    strcpy(new_process->command, command);
    new_process->next = bg_processes;
    bg_processes = new_process;
    return;
}

void remove_background_process(int pid) {
    ProcessStruct *current = bg_processes, *previous = NULL;
    while (current != NULL) {
        if (current->pid == pid) {
            if (previous == NULL) bg_processes = current->next;
            else previous->next = current->next;
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
    return;
}

void check_finished_background_processes() {
    int status, pid;
    ProcessStruct *current = bg_processes;
    while (current != NULL) {
        pid = waitpid(current->pid, &status, WNOHANG);
        if (pid > 0) {
            if (WIFEXITED(status)) printf("%s exited normally (%d)\n", current->command, current->pid);
            int temp_pid = current->pid;
            current = current->next;
            remove_background_process(temp_pid);
        }
        else current = current->next;
    }
    return;  
}


void terminate_all_background_processes() {
    ProcessStruct *current = bg_processes, *next;    
    while (current != NULL) {
        next = current->next;
        if (kill(current->pid, SIGTERM) == -1) printf("Failed to terminate process with PID %d: %s\n", current->pid, strerror(errno));
        current = next;
    }
    while (bg_processes != NULL) {
        ProcessStruct *temp = bg_processes;
        bg_processes = bg_processes->next;
        free(temp);
    }
}

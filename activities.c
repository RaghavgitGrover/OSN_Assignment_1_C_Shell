#include "headers.h"
extern ProcessStruct *bg_processes;

int cmp(const void *a, const void *b) {
    return *(int *)a - *(int *)b;
}

void activities_command() {
    ProcessStruct *current = bg_processes;
    int proc_id[MAX_LEN], idx = 0;
    while (current != NULL) {
        proc_id[idx++] = current->pid;
        current = current->next;
    }
    qsort(proc_id, idx, sizeof(int), cmp);
    for (int i = 0; i < idx; i++) {
        int status;
        int result = waitpid(proc_id[i], &status, WNOHANG | WUNTRACED);
        ProcessStruct *temp = bg_processes;
        while (temp != NULL) {
            if (temp->pid == proc_id[i]) {
                if (result == 0) printf("%d : %s - Running\n", temp->pid, temp->command);
                else if ((result > 0) && (WIFSTOPPED(status))) printf("%d : %s - Stopped\n", temp->pid, temp->command);
                break;
            }
            temp = temp->next;
        }
    }
    return;
}

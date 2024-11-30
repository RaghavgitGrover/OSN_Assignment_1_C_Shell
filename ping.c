#include "headers.h"
extern int signal_pid, signal_number;
extern int ctrlc_flag, ctrlz_flag;
extern ProcessStruct *fg_processes;
extern ProcessStruct *bg_processes;

void handle_sigint(int sig) {
    if (fg_processes != NULL) {
        kill(fg_processes->pid, SIGINT);
        printf("\nProcess with PID %d interrupted\n", fg_processes->pid);
    } 
    else printf("\nNo foreground process to interrupt\n");
}

void handle_sigtstp(int sig) {
    if (fg_processes != NULL) {
        printf("\nProcess with PID %d stopped\n", fg_processes->pid);
        add_background_process(fg_processes->pid, fg_processes->command);
        kill(fg_processes->pid, SIGTSTP);
        remove_fg_process(fg_processes->pid);
    } 
    else printf("\nNo foreground process to stop\n");
}

void setup_signal_handlers() {
    if (signal(SIGINT, handle_sigint) == SIG_ERR) printf("Error setting up SIGINT handler\n");
    else ctrlc_flag = 1;
    if (signal(SIGTSTP, handle_sigtstp) == SIG_ERR) printf("Error setting up SIGTSTP handler\n");
    else ctrlz_flag = 1;
}

void ping_command(char **args) {
    if (args[0] == NULL || args[1] == NULL) {
        printf("Usage: ping <pid> <signal_number>\n");
        return;
    }
    for (int i = 0; args[0][i] != '\0'; i++) if (!isdigit(args[0][i])) {
        printf("Enter a valid pid\n");
        return;
    }
    for (int i = 0; args[1][i] != '\0'; i++) if (!isdigit(args[1][i])) {
        printf("Enter a valid signal number\n");
        return;
    }
    int signal_pid = atoi(args[0]), signal_number = atoi(args[1]) % 32;
    if (kill(signal_pid, signal_number) == -1) printf("Process with PID %d does not exist\n", signal_pid);
    else printf("Signal %d sent to process with PID %d\n", signal_number, signal_pid);
    return;
}
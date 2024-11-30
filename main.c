#include "headers.h"
#include <pthread.h>
// #include <linux/time.h>
#include <time.h>

char username[MAX_LEN];
char hostname[MAX_LEN];
char home_dir[MAX_LEN];
char cwd[MAX_LEN];
char prev_dir[MAX_LEN];
char temp_dir[MAX_LEN];
char log_buffer[MAX_LOGS][MAX_LEN];
char log_file_path[MAX_LEN];
char myshrc_path[MAX_LEN];
char input_file[MAX_LEN];
char output_file[MAX_LEN];
char *url = "man.he.net";
char *port = "80";
int log_index = 0;
int log_count = 0;
int is_dir_only = 0;
int is_file_only = 0;
int execute_flag = 0;
int match_count = 0;
int show_hidden = 0;
int show_details = 0;
int signal_pid = 0;
int signal_number = 0;
int ctrlc_flag = 0;
int ctrlz_flag = 0;
int append_mode = 0;
int alias_error = 0;
int running = 1;
int flag_greater_than_2 = 0;
int time_greater_than_2 = 0;
char command_greater_than_2[MAX_LEN];

ProcessStruct *bg_processes = NULL;
ProcessStruct *fg_processes = NULL;
Alias *aliases = NULL;

const char *valid_commands_system[MAX_LEN] = {
    "useradd", "usermod", "userdel", "passwd", "groupadd", "groupdel", "fdisk", "mkfs", "fsck", "mount", "unmount", "ss", "ip", "pkill", "chgrp", "locate", "sh", "zsh", "bash", "clear", "vi", "mkdir", "rmdir", "cd", "echo", "export", "history", "pwd", "alias", "unalias", "jobs", "kill", "type", "test", "true", "false", "umask", "unset", "wait", "read", "trap", "ulimit", "set", "shift", "source", "disown", "logout", "exec", "enable", "dirs", "popd", "pushd", "declare", "let", "eval", "printf", "getopts", "times", "fc", "hash", "complete", "compgen", "compopt", "compadjust",
    "command", "shopt", "suspend", "ls", "cp", "mv", "rm", "cat", "grep", "find", "chmod", "chown", "head", "tail", "less", "more", "sort", "uniq", "cut", "paste", "wc", "tee", "df", "du", "touch", "stat", "ln", "id", "basename", "clear", "time", "bc", "yes", "factor", "expr", "sleep", "crontab", "at", "batch", "shutdown", "reboot", "halt", "poweroff", "sync", 
    "dirname", "md5sum", "sha256sum", "who", "whoami", "env", "printenv", "date", "cal", "uptime", "uname", "hostname", "traceroute", "ifconfig", "netstat", "killall", "xargs", "awk", "sed", "emacs", "gedit", "vim", "nano", "gcc", "g++", "make", "gdb", "cmake", "strace", "ltrace", "valgrind", "objdump", "ld", "nm", "curl", "wget", "ssh", "scp", "ftp", "telnet", "nslookup", "dig", "route", "iptables", 
    "ifup", "ifdown", "ps", "top", "htop", "df", "du", "free", "dmesg", "vmstat", "iostat", "pidstat",  "mpstat", "lsof", "iotop", "nice", "renice", "tar", "gzip", "bzip2", "zip", "unzip", "xz", "unxz", "7z", "rar", "unrar", "apt", "yum", "dnf", "pacman", "rpm", "zypper", "snap", "flatpak", "help", "man",
    NULL
};

const char *valid_commands_own[MAX_LEN] = {
    "hop", "reveal", "proclore", "seek", "log", "iMan", "activities", "ping", "fg", "bg", "neonate", "exit", NULL
};

bool is_valid_sequence(char *input) {
    char temp[MAX_LEN];
    strcpy(temp, input);
    int len = strlen(temp);
    if (temp[len - 1] == '\n') temp[len - 1] = '\0';
    char *saveptr = NULL;
    char *token = strtok_r(temp, ";", &saveptr);
    while (token != NULL) {
        char *sub_saveptr = NULL;
        char *command = strtok_r(token, " \t", &sub_saveptr);        
        if (command == NULL) return false;        
        int nature_of_command = is_valid_command(command);
        if (nature_of_command == 0) return false;
        token = strtok_r(NULL, ";", &saveptr);
    }
    return true;
}

void bg_command(char **args) {
    if (args[0] == NULL) {
        printf("Usage: bg <PID>\n");
        return;
    }
    for (int i = 0; args[0][i] != '\0'; i++) {
        if (!isdigit(args[0][i])) {
            printf("Enter a valid pid\n");
            return;
        }
    }
    int pid = atoi(args[0]);
    char command[MAX_LEN];
    ProcessStruct *temp = bg_processes;
    while (temp != NULL) {
        if (temp->pid == pid) {
            strcpy(command, temp->command);
            break;
        }
        temp = temp->next;
    }
    if (temp == NULL) {
        printf("No such process found with PID: %d\n", pid);
        return;
    }
    if (kill(pid, SIGCONT) == -1) {
        printf("Failed to resume process with PID %d: %s\n", pid, strerror(errno));
        return;
    }
    return;
}

void fg_command(char **args) {
    if (args[0] == NULL) {
        printf("Usage: fg <PID>\n");
        return;
    }
    for (int i = 0; args[0][i] != '\0'; i++) {
        if (!isdigit(args[0][i])) {
            printf("Enter a valid pid\n");
            return;
        }
    }
    int pid = atoi(args[0]);
    char command[MAX_LEN];
    ProcessStruct *temp = bg_processes;
    while (temp != NULL) {
        if (temp->pid == pid) {
            strcpy(command, temp->command);
            break;
        }
        temp = temp->next;
    }
    if (temp == NULL) {
        printf("No such process found with PID: %d\n", pid);
        return;
    }
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    if (kill(pid, SIGCONT) == -1) {
        printf("Failed to resume process with PID %d: %s\n", pid, strerror(errno));
        return;
    }
    int status;
    waitpid(pid, &status, WUNTRACED);
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    int time_taken = end_time.tv_sec - start_time.tv_sec;
    // if (time_taken > 2) printf("%s took %ds\n", args[0], time_taken);
    if (time_taken > 2) printf("%s with pid %d took %ds\n", command, pid, time_taken);
    if (WIFEXITED(status) && !ctrlc_flag) printf("%s exited normally with status %d\n", args[0], WEXITSTATUS(status));
    if (WIFSTOPPED(status)) {
        printf("Process with PID %d stopped: %s\n", pid, command);
        add_foreground_process(pid, command);
    }
    else if (WIFEXITED(status)) {
        // printf("Process with PID %d finished: %s\n", pid, command);
        remove_fg_process(pid);
    }
    else if (WIFSIGNALED(status)) {
        // printf("Process with PID %d terminated: %s\n", pid, command);
        remove_fg_process(pid);
    }
    return;
}

char *add_spaces(char *command, char *processed_command) {
    int i = 0, j = 0;
    while (command[i] != '\0') {
        if (command[i] == '<' || command[i] == '|') {
            processed_command[j++] = ' ';
            processed_command[j++] = command[i++];
            processed_command[j++] = ' ';
        } 
        else if (command[i] == '>') {
            if (command[i + 1] == '>') {
                processed_command[j++] = ' ';
                processed_command[j++] = '>';
                processed_command[j++] = '>';
                processed_command[j++] = ' ';
                i += 2;
            } 
            else {
                processed_command[j++] = ' ';
                processed_command[j++] = '>';
                processed_command[j++] = ' ';
                i++;
            }
        }
        else processed_command[j++] = command[i++];
    }
    processed_command[j] = '\0';
    return processed_command;
}

void *check_background_processes_thread(void *arg) {
    while (1) {
        pthread_mutex_t bg_process_mutex; 
        pthread_mutex_lock(&bg_process_mutex);
        check_finished_background_processes();
        pthread_mutex_unlock(&bg_process_mutex);
        sleep(1);  
    }
    return NULL;
}

char *remove_spaces(char *command) {
    char *processed_command = (char *)malloc(MAX_LEN * sizeof(char));
    int i = 0, j = 0;
    while (command[i] != '\0') {
        if (command[i] != ' ') processed_command[j++] = command[i++]; 
        else i++;
    }
    processed_command[j] = '\0';
    return processed_command;
}

int main() {
    if ((getlogin_r(username, MAX_LEN) == -1) || (gethostname(hostname, MAX_LEN) == -1) || (getcwd(home_dir, MAX_LEN) == NULL))        {
        printf("Error fetching details\n");
        printf("Closing the shell.....\n");
        return 0;
    }
    strcpy(prev_dir, home_dir);     
    snprintf(log_file_path, sizeof(log_file_path), "%s/logs.txt", home_dir); 
    snprintf(myshrc_path, sizeof(myshrc_path), "%s/.myshrc", home_dir);
    load_log();
    load_aliases();
    printf("\033[H\033[J");
    if (alias_error) printf("There exists an invalid alias in .myshrc file\n");

    pthread_t bg_thread;
    pthread_create(&bg_thread, NULL, check_background_processes_thread, NULL);

    while (1) {
        // check_finished_background_processes();
        prompt();
        ctrlc_flag = 0;
        ctrlz_flag = 0;
        setup_signal_handlers();
        char input[MAX_LEN];
        if (fgets(input, MAX_LEN, stdin) != NULL) {
            if (strlen(input) == 1) continue;
            char input_copy[MAX_LEN];
            char *log_command = strstr(input, "log");
            strcpy(input_copy, input);
            if (log_command == NULL && is_valid_sequence(input_copy)) add_to_log(input_copy);
            process_input(input);
        }   
        else {
            if (feof(stdin)) {
                printf("\n");
                terminate_all_background_processes();
                return 0;
            }
        }
    }
    pthread_join(bg_thread, NULL);
    return 0;
}






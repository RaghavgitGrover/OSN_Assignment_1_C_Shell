#include "headers.h"
#include <sys/select.h>
#include <termios.h>
#include <pthread.h>
  
extern int running;
struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);  
}

void* print_pids(void* arg) {
    int time_interval = *(int*)arg;
    while (running) {
        DIR *dir = opendir("/proc");
        if (dir == NULL) {
            printf("Couldn't open /proc\n");
            return NULL;
        }
        struct dirent *entry;
        int max_pid = -1;
        while ((entry = readdir(dir)) != NULL) {
            if (isdigit(entry->d_name[0])) {
                int pid = atoi(entry->d_name);
                if (pid > max_pid) max_pid = pid;
            }
        }
        closedir(dir);
        if (max_pid != -1) printf("%d\n", max_pid);
        sleep(time_interval);  
    }
    return NULL;
}

void neonate_command(char **args) {
    if (args[1] == NULL || strcmp(args[0], "-n" ) != 0) {
        printf("Usage: neonate -n <time>\n");
        return;
    }
    for (int i = 0; args[1][i] != '\0'; i++) {
        if (!isdigit(args[1][i])) {
            printf("Enter a valid value of time\n");
            return;
        }
    }
    int time = atoi(args[1]);
    running = 1;
    tcgetattr(STDIN_FILENO, &orig_termios); 
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);  
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);  
    atexit(disable_raw_mode);
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, print_pids, &time);
    while (1) {
        char c = getchar();
        if (c == 'x') {
            running = 0;  
            break;
        }
    }
    pthread_join(thread_id, NULL);
    disable_raw_mode();  
}
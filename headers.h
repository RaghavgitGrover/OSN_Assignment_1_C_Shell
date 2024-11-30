#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <signal.h>
#include <grp.h>
#include <dirent.h>
#include <pwd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <glob.h>
#include <fts.h>
#include <ftw.h>
#include <fnmatch.h>
#include <assert.h>
#include <ctype.h>
#include <signal.h>
#include <poll.h>
// #include <bits/pthreadtypes.h>
// #include <pthread.h>

#include "prompt.h" 
#include "input.h"
#include "command.h"
#include "hop.h"
#include "log.h"
#include "reveal.h"
#include "proclore.h"
#include "seek.h"
#include "bg.h"
#include "iMan.h"
#include "alias.h"
#include "activities.h"
#include "ping.h"
#include "fg.h"
#include "redirect.h"
#include "pipe.h"
#include "pipdirect.h"
#include "neonate.h"

#define BLUE  "\x1b[34m"
#define GREEN "\x1b[32m"
#define WHITE "\x1b[37m"
#define RESET "\x1b[0m"

#define _POSIX_C_SOURCE 200809L
#define MAX_LOGS 15
#define MAX_LEN 4096

typedef struct ProcessStruct {
    int pid;
    char command[MAX_LEN];
    struct ProcessStruct *next;
} ProcessStruct;

typedef struct Alias {
    char *alias_name;
    char *command;
    struct Alias *next;
} Alias;

void fg_command(char **args);
void bg_command(char **args);
char *add_spaces(char *command, char *processed_command);
char *remove_spaces(char *command);
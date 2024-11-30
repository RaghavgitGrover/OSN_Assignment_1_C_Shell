#ifndef PING_H
#define PING_H

void handle_sigint(int sig);
void handle_sigtstp(int sig);
void setup_signal_handlers();
void ping_command(char **args);

#endif
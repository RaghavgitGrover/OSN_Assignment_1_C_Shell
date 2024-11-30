#ifndef LOG_H
#define LOG_H

void load_log();
void save_log();
void add_to_log(char *command);
void log_command(char **args);

#endif
#ifndef BG_H
#define BG_H

void add_background_process(int pid, char *command);
void check_finished_background_processes();
void remove_background_process(int pid);
void terminate_all_background_processes();

#endif
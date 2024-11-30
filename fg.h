#ifndef FG_H
#define FG_H

void add_foreground_process(int pid, char *command);
void remove_fg_process(int pid);

#endif
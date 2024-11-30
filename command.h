#ifndef COMMAND_H
#define COMMAND_H

int is_valid_command(char *command);
void execute_command(char **args, int background, int nature_of_command);

#endif

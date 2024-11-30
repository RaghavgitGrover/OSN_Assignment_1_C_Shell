#include "headers.h"

extern int alias_error;
extern Alias *aliases;
extern myshrc_path[MAX_LEN];

void load_aliases() {
    alias_error = 0;
    FILE *file = fopen(myshrc_path, "r");
    if (file == NULL) {
        printf("Could not open .myshrc file\n");
        return;
    }
    char line[MAX_LEN];
    while (fgets(line, sizeof(line), file) != NULL) {
        char *trimmed_line = trim_whitespace(line);
        trimmed_line[strcspn(trimmed_line, "\n")] = 0;
        if (strlen(trimmed_line) == 0 || strchr(trimmed_line, '#') != NULL) continue;
        char *alias_key = strtok(trimmed_line, "=");
        char *alias_value = strtok(NULL, "=");
        if ((alias_key == NULL || alias_value == NULL)) {
            alias_error = 1;
            continue;
        }
        alias_key = trim_whitespace(alias_key);
        alias_value = trim_whitespace(alias_value);
        Alias *new_alias = (Alias *)malloc(sizeof(Alias));
        new_alias->alias_name = (char *)malloc(strlen(alias_key) + 1);
        strcpy(new_alias->alias_name, alias_key);
        new_alias->command = (char *)malloc(strlen(alias_value) + 1);
        strcpy(new_alias->command, alias_value);
        new_alias->next = aliases;
        aliases = new_alias;
    }
    fclose(file);
}
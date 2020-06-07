#define WSH_RL_BUFSIZE 1024

#define WSH_TOK_BUFSIZE 64
#define WSH_TOK_DELIM " \t\r\n\a"

/*
 * Function Declarations for builtin shell commands:
 */
int wsh_cd(char **args);
int wsh_help(char **args);
int wsh_cat(char **args);
int wsh_exit(char **args);

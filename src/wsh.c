#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../inc/wsh.h"


/*
 * List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};


int (*builtin_func[]) (char **) = {
    &wsh_cd,
    &wsh_help,
    &wsh_exit
};


int wsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}


/*
 * Builtin function implementations.
 */

/*
 * @brief Bultin command: change directory.
 * @param args List of args.  args[0] is "cd".  args[1] is the directory.
 * @return Always returns 1, to continue executing.
 */
int wsh_cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "wsh: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("wsh");
        }
    }
    return 1;
}


/*
 * @brief Builtin command: print help.
 * @param args List of args.  Not examined.
 * @return Always returns 1, to continue executing.
 */
int wsh_help(char **args)
{
    int i;
    printf("Wake Shell WSH\n");
    printf("The following are built in:\n");
    
    for (i = 0; i < wsh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }
    
    printf("Use the man command for information on other programs.\n");
    return 1;
}


/*
 * @brief Builtin command: exit.
 * @param args List of args.  Not examined.
 * @return Always returns 0, to terminate execution.
 */
int wsh_exit(char **args)
{
    return 0;
}


/*
 * @brief Launch a program and wait for it to terminate.
 * @param args Null terminated list of arguments (including program).
 * @return Always returns 1, to continue execution.
 */
int wsh_launch(char **args)
{
    pid_t pid, wpid;
    int status;
    
    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("wsh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("wsh");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    
    return 1;
}


/*
 * @brief Execute shell built-in or launch program.
 * @param args Null terminated list of arguments.
 * @return 1 if the shell should continue running, 0 if it should terminate
 */
int wsh_execute(char **args)
{
    int i;

    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }

    for (i = 0; i < wsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return wsh_launch(args);
}


/*
 * @brief Read a line of input from stdin.
 * @return The line from stdin.
 */
char *wsh_read_line(void)
{
    int bufsize = WSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "wsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Read a character
        c = getchar();

        // If we hit EOF, replace it with a null character and return.
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        // If we have exceeded the buffer, reallocate.
        if (position >= bufsize) {
            bufsize += WSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "wsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}


/*
 * @brief Split a line into tokens (very naively).
 * @param line The line.
 * @return Null-terminated array of tokens.
 */
char **wsh_split_line(char *line)
{
    int bufsize = WSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;
    
    if (!tokens) {
        fprintf(stderr, "wsh: allocation error\n");
        exit(EXIT_FAILURE);
    }
    
    token = strtok(line, WSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;
        
        if (position >= bufsize) {
            bufsize += WSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "wsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        
        token = strtok(NULL, WSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}


/*
 * @brief Loop getting input and executing it.
 */
void wsh_loop(void)
{
    char *wsh_prompt;
    char *line;
    char **args;
    int status;

    if (getuid() == 0)
        wsh_prompt = "wsh # ";
    else
        wsh_prompt = "wsh $ ";

    do {
        printf("%s", wsh_prompt);

        line = wsh_read_line();
        args = wsh_split_line(line);
        status = wsh_execute(args);

        free(line);
        free(args);
    } while (status);
}


/*
 * @brief Main entry point.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return status code
 */
int main(int argc, char **argv)
{
    /* Start shell loop */
    wsh_loop();
    return EXIT_SUCCESS;
}

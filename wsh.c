#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


#define WSH_RL_BUFSIZE 1024
char *lsh_read_line(void)
{
    int bufsize = WSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;
    
    if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
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
            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}


/**
   @brief Loop getting input and executing it.
 */
void lsh_loop(void)
{
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


/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
    /* Start shell loop */
    wsh_loop();
    return 0;
}

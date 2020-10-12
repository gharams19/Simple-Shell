#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define CMDLINE_MAX 512
#define ARG_MAX 16
#define PATH_MAX 4096 // Linux defines PATH_MAX 4096 bytes
#define TOKEN_MAX 32

void system_loop(void);
char *allocate_cmd(void);
char **cmd_parser(char *line);
int execute(char **args);

int system_call(char **args);



int main(int argc, char **argv)
{       
        system_loop();
        return EXIT_SUCCESS;
}

void system_loop(void){
        char *line;
        char **args;
        int retval;

        do {
                /* Print prompt */
                printf("sshell$ ");
                fflush(stdout);

                line = allocate_cmd();
                args = cmd_parser(line);
                retval = execute(args);

                free(line);
                free(args);                    
      } while(retval);
}

// Reading cmd and allocate buffer block
char *allocate_cmd(void){
        int buffersize = CMDLINE_MAX;
        char *buffer = malloc(sizeof(char) * buffersize);

        if(!buffer){
                fprintf(stderr, "allocation error\n");
                exit(EXIT_FAILURE);
        }

        int character;
        int position = 0;
        while(1){
                character = getchar();
                if (character == '\n'){
                        buffer[position] = '\0';
                        return buffer;
                } else{
                        buffer[position] = character;
                }
                position++;

                // Assumed max length of cmd line never exceeds 512 chars
                if (position >= buffersize){
                        fprintf(stderr, "buffer exceeded\n");
                        exit(EXIT_FAILURE);
                }
        }
}

// Parsing the command line after buffer, memory allocation
// result of parsing the cmd should be instance of data structure contains all the information - arguments
char **cmd_parser(char *line){
        int buffersize = TOKEN_MAX;
        char **tokens = malloc(buffersize * sizeof(char*));
        char *token;        

        if(!tokens){
                fprintf(stderr, "tokenization error\n");
                exit(EXIT_FAILURE);
        }

        int position = 0;
        token = strtok(line, " \t\r\n");
        while (token != NULL){
                tokens[position] = token;
                position++;
        }

        tokens[position] = NULL;
        return tokens;
}
/*
static struct cmd_line{
        const char w;
        const char t;
        const char r;
        const char n;
}

my_cmd_line(struct cmd_line *obj, int b){
        obj->a = b;
}
*/

int execute(char **args){
        if(args[0] == NULL)
                return 1;
        
        // Check for the built in functions
        //for(int i = 0; i <  )
        //        

        return system_call(args);
}

int system_call(char **args){
        pid_t pid;

        pid = fork();
        if (pid == 0){
                if(execvp(args[0], args) == -1)
                        perror("execvp");
                exit(1);
                
        } else if (pid < 0){
                perror("fork");
                exit(1);
        } else{
                int status;
                waitpid(pid, &status, 0);
                printf("Child returned %d\n", WEXITSTATUS(status));
        }

        return EXIT_SUCCESS;
}




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define CMDLINE_MAX 512
#define PATH_MAX 4096 // Linux defines PATH_MAX 4096 bytes
#define TOKEN_MAX 32

char *read_cmd(void){
        // memory allocation
        char *cmd = malloc(sizeof(char) * CMDLINE_MAX);
        //int cmd_len = strlen(cmd);
        //fgets(cmd, CMDLINE_MAX, stdin);
        
        /* Print command line if stdin is not provided by terminal */
        if (!isatty(STDIN_FILENO)) { // isatty(fd) test whether a fd is open fd refers to a terminal
                printf("%s", cmd);
                fflush(stdout);
        }

        if(!cmd){
                //fprintf(stderr, "ERROR: malloc");
                perror("ERROR: malloc");
                exit(EXIT_FAILURE);
        }

        // reading the cmd line 
        int offset;
        do{
                offset = read(STDIN_FILENO, cmd, CMDLINE_MAX);
                //printf("%s%d\n", cmd, offset);
                if(offset == -1) {// at the end of the txt
                        //fprintf(stderr, "ERROR: read cmd_line");
                        perror("ERROR: read cmd_line");
                        exit(EXIT_FAILURE);
                }
                // reset cursor pointer to NULL
                cmd[offset] = '\0';
                
                return cmd;
        }while(1);
}

// parsing the cmd_line using string tokenization 
// ---------------------------------------------- USE struct object 

/* 
#define delimiter = " \t\r\n\a";
struct parser{
        char *s_token;
        char **token;
        int buffer;
        int offset;
};
*/
char **parse_cmd(char *cmd){
        // memory allocation
        char *s_token;
        char **token = malloc(sizeof(char*) * TOKEN_MAX);
        if(!token){
                //fprintf(stderr, "ERROR: malloc");
                perror("ERROR: malloc");
                exit(EXIT_FAILURE);
        }

        // ref: https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
        int pointer = 0;
        s_token = strtok(cmd, " \t\r\n\a"); // common delimiter
        while(s_token != NULL){
                token[pointer] = s_token;
                pointer++;
                s_token = strtok(NULL, " \t\r\n\a");
        }
        token[pointer] = NULL;
        return token;
}

int execute_cmd(char **args){
        if(args[0] == NULL)
                return EXIT_FAILURE;
        // ---------------------------- Check whether it is Builtin command or not
        /* Builtin command */
        // for(unsigned long i = 0; i < 4; i++) pwd, cd, exit, sls
        // if (strcmp(args[0], builtin commands) == 0)
        // execute built in - return 
        if (!strcmp(args[0], "exit")) {  // string compare, equal == 0
                fprintf(stderr, "Bye...\n");
                free(args[0]);
                return EXIT_FAILURE;
        }

        pid_t pid;
        pid = fork();
        if(pid==0){ // child process
                if(execvp(args[0], args)==-1)
                        return EXIT_FAILURE;
                return EXIT_SUCCESS;
        } else if(pid > 0){ // parent
                int status;
                waitpid(pid, &status, 0);
                if(WEXITSTATUS(status) == -1)
                        return EXIT_FAILURE;
                return EXIT_SUCCESS;
        } else{ // forking error
                perror("fork");
                exit(1);
        }

        return EXIT_SUCCESS;
}

//void execute_pipe(char **args1, char**args2){
//
//}

int main(void) 
{
        //char cmd[CMDLINE_MAX]; 
        char *cmd; 
        char **token;
        
        while (1) {
                //char *nl;
                int retval;

                /* Print prompt */
                printf("sshell$ ");
                fflush(stdout);

                /* Get command line */
                //fgets(cmd, CMDLINE_MAX, stdin); // reads a line from stdin stream stores into cmd as string
                cmd = read_cmd();

                /* Print command line if stdin is not provided by terminal */
                //if (!isatty(STDIN_FILENO)) { // isatty(fd) test whether a fd is open fd refers to a terminal
                //        printf("%s", cmd);
                //        fflush(stdout);
                //}

                /* Remove trailing newline from command line */
                //nl = strchr(cmd, '\n'); // returns pointer to first occurrnece of char,\n
                //if (nl)
                //        *nl = '\0'; // termination of a char string

                        
                /* Regular command */
                //retval = system(cmd);
                token = parse_cmd(cmd);
                retval = execute_cmd(token); // system(cmd);

                if(retval==0)
                        fprintf(stderr, "Return status value for '%s': %d\n",
                        cmd, retval);
                else
                        break;
        }

        return EXIT_SUCCESS;
}

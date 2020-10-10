#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#define PATH_MAX        4096 
#define CMDLINE_MAX 512


char * get_path(char* cmd, int cmd_length) {
    char path[PATH_MAX];

    int j = 0;

    for(unsigned long i = cmd_length; i < strlen(cmd); i++) {
        if(cmd[i] != ' ') {
            path[j] = cmd[i];
            j++;
        }
    }
    char* path_cmd = path;
    return path_cmd;
}
void execute_builtin_commands(char* cmd){
    char cwd[PATH_MAX];
    
    if(strstr(cmd,"pwd") != NULL) {
        if(strlen(cmd) > 3) {
            char* path = get_path(cmd,3);
            if(chdir(path) != 0) {
                perror("pwd failed");
            };
        }
        getcwd(cwd, sizeof(cwd));
        fprintf(stdout, "%s\n", cwd);
        
    }
    else if(strcmp(cmd,"exit") == 0) {
        fprintf(stderr, "Bye...\n");
        exit(0);
    }
    else if(strstr(cmd,"cd") != NULL) {
        char* path = get_path(cmd,2);
        if(chdir(path) != 0) {
            perror("cd failed");
        };
    }
    else if(strcmp(cmd,"sls") == 0) {
        DIR *dirp;
        struct dirent *dp;

        dirp = opendir(".");
        while((dp = readdir(dirp)) != NULL) {
            struct stat sb;
            stat(dp->d_name, &sb);
            fprintf(stdout, "%s (%lld bytes)\n", dp->d_name, sb.st_size);
        }
    }

}
int main(void)
{
        char cmd[CMDLINE_MAX];

        while (1) {
                char *nl;
                // int retval;

                /* Print prompt */
                printf("sshell$ ");
                fflush(stdout);

                /* Get command line */
                fgets(cmd, CMDLINE_MAX, stdin);

                /* Print command line if stdin is not provided by terminal */
                if (!isatty(STDIN_FILENO)) {
                        printf("%s", cmd);
                        fflush(stdout);
                }

                /* Remove trailing newline from command line */
                nl = strchr(cmd, '\n');
                if (nl)
                        *nl = '\0';

                /* Builtin command */
                if (!strcmp(cmd, "exit")) {
                        fprintf(stderr, "Bye...\n");
                        break;
                }
                if(strcmp(cmd, "pwd") || strcmp(cmd, "exit")) {
                    execute_builtin_commands(cmd);
                    fprintf(stdout, "+ completed '%s' [0]\n", cmd);
                }

                /* Regular command */
                // retval = system(cmd);
                // fprintf(stdout, "Return status value for '%s': %d\n",
                //         cmd, retval);
        }

        return EXIT_SUCCESS;
}
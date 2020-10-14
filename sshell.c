#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#define PATH_MAX        4096 
#define CMDLINE_MAX 512

void get_filename_and_output(char* cmd, unsigned long pos1, unsigned long pos2, char output[], char filename[], int i) {
    int j = 0; 
    for(unsigned long i = pos1+1; i < pos2; i++) {
        output[j] = cmd[i];
        j++;  
    }
    int n = 0;
    for(unsigned long l = pos2+i; l < strlen(cmd); l++) {
        if(cmd[l] != ' ') {
            filename[n] = cmd[l];
            n++;
        } 
    }
    filename[n] = '\0';
}
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
int execute_builtin_commands(char* cmd){
    char cwd[PATH_MAX];
    int ret = 0;
    if(strstr(cmd,"pwd") != NULL) {
        
        if(getcwd(cwd, sizeof(cwd)) != NULL) {
            fprintf(stdout, "%s\n", cwd);
            ret = 0;
        }
        else {
            ret = 1;
            perror("Error: pwd failed");

        }
        
        
    }
    else if(strstr(cmd,"exit") != NULL) {
        fprintf(stderr, "Bye...\n");
        ret = 0;
    }
    else if(strstr(cmd,"cd") != NULL) {
        char* path = get_path(cmd,2);
        ret = chdir(path);
        if(ret != 0) {
            perror("Error: cannot cd into directory");
            ret = 1;
        }
    }
    else if(strstr(cmd,"sls") != NULL) {
        DIR *dirp;
        struct dirent *dp;

        dirp = opendir(".");
        while((dp = readdir(dirp)) != NULL) {
            struct stat sb;
            stat(dp->d_name, &sb);
            fprintf(stdout, "%s (%lld bytes)\n", dp->d_name, sb.st_size);
            ret = 0;
        }
    }
    return ret;

}
void output_redirection(char* cmd) {
    char* cmd_char;
    char filename[PATH_MAX];
    char temp_path[PATH_MAX+1];
    char output[PATH_MAX];
    char* path;
    int fds;
    int i =0;
    unsigned long pos1;
    
   
    cmd_char = strchr(cmd, '>');
    unsigned long pos2 = (unsigned long)(cmd_char - cmd);
    
    if(cmd[pos2+1] == '>') {
        i = 2;
    }
    else {
        i = 1;
    }
    cmd_char = strchr(cmd, '.');
    pos1 = (unsigned long)(cmd_char - cmd) + 1;
    
    get_filename_and_output(cmd,pos1,pos2,output,filename,i);
    

    realpath(output,temp_path);
    path = temp_path;
    
    path[strlen(path)-1] = '\0';

    if(i == 2) {
        fds = open(filename, O_RDWR|O_CREAT|O_APPEND, 0600);
    }
    else {
        fds = open(filename, O_RDWR|O_CREAT, 0600);
    }
    
    if (fds == -1) { 
        perror("Error: Cannot open file");  
    }
    if(dup2(fds,fileno(stdout)) == -1) {
        perror("Error: Cannot redirect output");
    }
    if(strstr(cmd,"./") != NULL) {
        char* args[]  = {path,"", NULL};
        pid_t pid = fork();
        if(pid == 0) {
            execv(path,args);
        }
        else if(pid > 0) {
            int status;
            wait(&status);
            fflush(stdout); 
            close(fds);

            dup2(1, fileno(stdout));

            close(1);  
            return EXIT_SUCCESS;

        }
        else{ 
            perror("Error: fork");
        }
        

    }

    
    
            
}
int main(void)
{
        char cmd[CMDLINE_MAX];
        int ret;

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

                if(strstr(cmd,"cd") != NULL || strstr(cmd,"pwd") != NULL || strstr(cmd,"sls") != NULL || strstr(cmd,"exit") != NULL){
                    ret = execute_builtin_commands(cmd);
                    fprintf(stdout, "+ completed '%s' [%d]\n", cmd, ret);
                    if(strstr(cmd,"exit") != NULL) {
                        exit(0);
                    }
                   
                }

                if(strstr(cmd,">") != NULL){
                    output_redirection(cmd);

                } 

                /* Regular command */
                // retval = system(cmd);
                // fprintf(stdout, "Return status value for '%s': %d\n",
                //         cmd, retval);
        }

        return EXIT_SUCCESS;
}
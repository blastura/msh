#include "execute.h"
#include "parser.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/wait.h>

# define WRITE_END  1
# define READ_END 0

// #include <signal.h> // Should be used for signal handling

int shell(FILE *restrict stream_in);

int main(int argc, char* const argv[]) {
    if (argc == 1) {
        return shell(stdin);
    } else {
        printf("TODO: implement");
    }
}

int shell(FILE *restrict stream_in) {
    prompt();
    char line[MAXLINELEN];
    command comLine[MAXCOMMANDS];
    
    while (fgets(line, MAXLINELEN, stream_in) != NULL) {
        
        char eline[MAXLINELEN];
        eline[0] = 0;
        
        printf("line: %s\n", line);
        printf("eline: %s\n", eline);
        
        expand(line, eline);
        int nrCommands = parse(eline, comLine);
        
        /*** print stuff **********/
        printf("nrCommands: %d\n", nrCommands);
        
        for (int i = 0; i < nrCommands; i++) {
            printf("New Commando ------------------------");
            command cmd = comLine[i];
            printf("argc: %d\n", cmd.argc);
            
            for (int j = 0; j < cmd.argc; j++) {
                printf("argv: %s\n", cmd.argv[j]);
            }

            printf("infile: %s\n", cmd.infile);
            printf("outfile: %s\n", cmd.outfile);
            printf("internal: %d\n", cmd.internal);
        }

        /* Do stuff *******************/
        
        /* Will contain one pipe for communication between all
           commands */
        int pipesArray[nrCommands - 1][2];
        
        /* Will contain pid for every forked process */
        int pidArray[nrCommands];
        
        for (int i = 0; i < nrCommands; i++) {
            command cmd = comLine[i];

            if (pipe(pipesArray[i]) != 0 ) {
                perror("Failed to create pipe: ");
                exit(-1);
            }
                
            // Create new process
            switch (pidArray[i] = fork()) {
                case -1:
                    fprintf(stderr, "fork() failed\n");
                    break;
                case 0:
                    printf("barn: %s\n", cmd.argv[0]);
                    /* Child */
                    
                    // Read input from previous command
                    if (i > 0
                        && dup2(pipesArray[i - 1][READ_END], STDIN_FILENO) < 0) {
                        perror(*cmd.argv);
                        close(pipesArray[i - 1][READ_END]);
                        return 1;
                    }
                    // Close pipe
                    close(pipesArray[i - 1][READ_END]);
                    
                    // Write output to next commands
                    if (i + 1 < nrCommands
                        && dup2(pipesArray[i][WRITE_END], STDOUT_FILENO) < 0) {
                        perror(*cmd.argv);
                        close(pipesArray[i][WRITE_END]);
                        return 1;
                    }
                    // Close pipe
                    close(pipesArray[i][WRITE_END]);
                        

                    if (execvp(*cmd.argv++, cmd.argv) < 0 ) {
                        fprintf(stderr,"Kunde inte göra en exec\n");
                        perror("Exec");
                        exit(1);
                    }
                    printf("Barnet har process id %d medans getpid() har %d\n",
                           pidArray[i], getpid());
                    return 0;
                default:
                    /* Föräldern */
                    printf("Barnet fick process id %d medans föräldern har %d\n",
                           pidArray[i], getpid());

            }
        }
        
        int status[nrCommands];
        for (int i = 0; i < nrCommands; i++) {
            printf("i = %d. Waiting for pid: %d, getpid(): %d\n", i, pidArray[i], getpid());
            //wait(&status[i]);
            waitpid(pidArray[i], &status[i], WNOHANG);
            printf("Parent wait got status from child: %d, status: %d\n",
                   pidArray[i], status[i]);
            /*                         printf("WEXITSTATUS: %d\n",WEXITSTATUS(status)); */
            /*                         printf("WIFEXITED: %d\n",WIFEXITED(status)); */
            /*                         printf("WIFSIGNALED: %d\n",WIFSIGNALED(status)); */
            /*                         printf("WIFSTOPPED: %d\n",WIFSTOPPED(status)); */
        }

        /* Custom commands *****/
        if (strcmp(line, "exit\n") == 0) {
            exit(0);
        } else {
            prompt();
        }
    }
    printf("shell method ends, pid: %d\n", getpid());
    return 0;
}

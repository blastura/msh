#include "execute.h"
#include "parser.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/wait.h>

// #include <signal.h> // Should be used for signal handling

int shell(FILE *restrict stream_in);

int main(int argc, char* const argv[]) {
    printf("STDIN: %d\n", STDIN_FILENO);
    printf("STDOUT: %d\n", STDOUT_FILENO);
    
    
    
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
        int pipesArray[nrCommands][2];
        
        /* Will contain pid for every forked process */
        int pidArray[nrCommands];
        
        /* Fork every command */
        for (int i = 0; i < nrCommands; i++) {
            printf("Doing command for i: %d\n", i);
            command cmd = comLine[i];

            // Create a pipe for command
            if (pipe(pipesArray[i]) != 0 ) {
                printf("couldnt create pipe________________");
                perror("Failed to create pipe: ");
                exit(-1);
            }
            
            // Create new process
            switch (pidArray[i] = fork()) {
                case -1: // Fork failed
                    fprintf(stderr, "fork() failed\n");
                    break;
                case 0: // Child
                    printf("barn: %s\n", cmd.argv[0]);
                    //sleep(1);
                    // Read input from previous command
                    if (i > 0) { // every command but the first
                        fprintf(stdout, "Redir in, i: %d\n", i);
                        if (dupPipe(pipesArray[i - 1], READ_END, STDIN_FILENO) < 0) {
                            _exit(-1);
                        }
                    }

                    // Write output to next commands
                    if (i + 1 < nrCommands) { // any command but the last
                        fprintf(stderr, "Redir out, i: %d\n", i);
                        
                        if (dupPipe(pipesArray[i], WRITE_END, STDOUT_FILENO) < 0) {
                            _exit(1);
                        }
                    } else if (i + 1 == nrCommands) { // Last command
                        
                    }
                    
                    if (execvp(*cmd.argv++, cmd.argv) < 0 ) {
                        fprintf(stderr,"Kunde inte göra en exec\n");
                        perror("Exec");
                        _exit(1); // Don't flush parents files
                    }
                    
                    printf("-----------------------Barnet har process id %d medans getpid() har %d\n",
                           pidArray[i], getpid());
                    exit(0);
                default: // Parent
                    printf("Barnet fick process id %d medans föräldern har %d, i: %d\n",
                           pidArray[i], getpid(), i);
            }
        }
        
        for (int i = 0; i < nrCommands; i++) {
            close(pipesArray[i][READ_END]);
            close(pipesArray[i][WRITE_END]);
        }
        
        for (int i = 0; i < nrCommands; i++) {
            /* close(pipesArray[i][READ_END]); */
            /* close(pipesArray[i][WRITE_END]); */

            int status;
            printf("i = %d. Waiting for pid: %d, getpid(): %d\n", i, pidArray[i], getpid());
            //wait(&status[i]);
            
            while (waitpid(pidArray[i], &status, 0) < 0) {
                perror("Error waiting for process:");
            }
            
            printf("Parent wait got status from child: %d, status: %d\n",
                   pidArray[i], status);
            /* printf("WEXITSTATUS: %d\n",WEXITSTATUS(status)); */
            /* printf("WIFEXITED: %d\n",WIFEXITED(status)); */
            /* printf("WIFSIGNALED: %d\n",WIFSIGNALED(status)); */
            /* printf("WIFSTOPPED: %d\n",WIFSTOPPED(status)); */
        }

        /* Custom commands *****/
        if (strcmp(line, "exit\n") == 0) {
            exit(0);
        } else {
            fflush(stdout);
            prompt();
        }
    }
    printf("shell method ends, pid: %d\n", getpid());
    return 0;
}

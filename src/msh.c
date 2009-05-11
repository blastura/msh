#include "execute.h"
#include "parser.h"

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
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
        //int pipesArray[nrCommands][2];
        int prevPipe[2];
        int currPipe[2];
        
        /* Will contain pid for every forked process */
        int pidArray[nrCommands];
        
        /* Fork every command */
        for (int i = 0; i < nrCommands; i++) {
            printf("Doing command for i: %d\n", i);
            command cmd = comLine[i];
            
            /* Close pipes that are open, this will be true for every
             * command but the first two */
            if (i > 1) {
                close(prevPipe[READ_END]);
                close(prevPipe[WRITE_END]);
            }
            if (i > 0) {
                prevPipe[READ_END] = currPipe[READ_END];
                prevPipe[WRITE_END] = currPipe[WRITE_END];
            }
            
                // Create a pipe for command
            if (pipe(currPipe) != 0 ) {
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

                    // Read input from previous command
                    if (i > 0) { // every command but the first
                        fprintf(stderr, "Redir in, i: %d\n", i);
                        fprintf(stderr, "prevPipe  i, read: %d, %d\n", i, prevPipe[READ_END]);
                        fprintf(stderr, "prevPipe  i, write: %d, %d\n", i, prevPipe[WRITE_END]);
                        
                        if (dupPipe(prevPipe, READ_END, STDIN_FILENO) < 0) {
                            _exit(-1);
                        }
                    } else if (i == 0) { // First command
                        /* close(currPipe[READ_END]); */
                        /* close(currPipe[WRITE_END]); */
                    }

                    // Write output to next commands
                    if (i + 1 < nrCommands) { // every command but the last
                        fprintf(stderr, "Redir out, i: %d\n", i);
                        fprintf(stderr, "currPipe  i, read: %d, %d\n", i, currPipe[READ_END]);
                        fprintf(stderr, "currPipe  i, write: %d, %d\n", i, currPipe[WRITE_END]);

                        if (dupPipe(currPipe, WRITE_END, STDOUT_FILENO ) < 0) {
                            _exit(1);
                        }
                    } else if (i + 1 == nrCommands) { // Last command
                        /* close(currPipe[READ_END]); */
                        /* close(currPipe[WRITE_END]); */
                    }
                    
                    // Hur får jag reda på vad som är aktuell stdout efter kopiering
                    fprintf(stderr, "=========================stdoutnr: %d\n", fileno(stdout));
                    
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
        
        // Close pipes
        close(currPipe[READ_END]);
        close(currPipe[WRITE_END]);
        
        if (nrCommands > 1) {
            close(prevPipe[READ_END]);
            close(prevPipe[WRITE_END]);
        }
        
        for (int i = 0; i < nrCommands; i++) {
            int status;
            printf("i = %d. Waiting for pid: %d, getpid(): %d\n", i, pidArray[i], getpid());
            //wait(&status[i]);
            
            while (waitpid(pidArray[i], &status, 0) < 0) {
                if (errno != EINTR) {
                    perror("Error waiting for process:");
                    break;
                } else {
                    fprintf(stderr, "Looopppp\n");
                }
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
            prompt();
        }
    }
    printf("shell method ends, pid: %d\n", getpid());
    return 0;
}

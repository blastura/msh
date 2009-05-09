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
        
        for (int i = 0; i < nrCommands; i++) {
            if ((i + 1) < nrCommands) { /* If there is a pipe to redirect output to */
                command cmd1 = comLine[i];
                command cmd2 = comLine[i + 1];
                FILE *fpout, *fpin;
                char buf[MAXLINELEN];
                
                // Read from cmd1
                if ((fpout = popen(cmd1.argv[0], "r")) == NULL) {
                    fprintf(stderr, "Error: Unable to popen() on line %d\n", __LINE__);
                    return -1;
                }
                
                // Write to cmd2
                if ((fpin = popen(cmd2.argv[0], "w")) == NULL) {
                    fprintf(stderr, "Error: Unable to popen() on line %d\n", __LINE__);
                    pclose(fpin);
                    return -1;      
                }
                
                while (fgets(buf, MAXLINELEN, fpout) != NULL) {
                    /* Send the recieved output as input to the command in argv[2]
                       via the FILE pointer fpin */
                    if (fputs(buf, fpin) == EOF) {
                        fprintf(stderr, "fputs()\n");
                        return -1;
                    }
                }
                pclose(fpout);
                pclose(fpin);

            } else { /* Last or single command */
                pid_t pid;
                switch (pid = fork()) {
                    case -1:
                        fprintf(stderr, "Kunde inte köra fork");
                        break;
                    case 0:
                        /* Barnet */
                        printf("Barnet");
                        if (execvp(*comLine->argv++, comLine->argv) < 0 ) {
                            fprintf(stderr,"Kunde inte göra en exec\n");
                            perror("Exec:");
                            exit(1);
                        }
                        printf("Barnet har process id %d medans getpid() har %d\n",
                               pid, getpid());
                        break;
                    default:
                        /* Föräldern */
                        printf("Barnet fick process id %d medans föräldern har %d\n",
                               pid, getpid());
                
                        int status;
                        wait(&status);
                        printf("Föräldern status: %d\n", status);
                        printf("Parent signing off. Child exited with status %d \n",status);
                        printf("WEXITSTATUS: %d\n",WEXITSTATUS(status));
                        printf("WIFEXITED: %d\n",WIFEXITED(status));
                        printf("WIFSIGNALED: %d\n",WIFSIGNALED(status));
                        printf("WIFSTOPPED: %d\n",WIFSTOPPED(status));
                }
            }
        }
        
        if (strcmp(line, "quit\n") == 0) {
            exit(0);
        } else {
            prompt();
        }
    }
    return 0;
}

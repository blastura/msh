/*
 * Name: Anton Johansson
 * Mail: dit06ajn@cs.umu.se
 * Time-stamp: "2009-05-28 11:48:58 anton"
 */

#include "msh.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For Solaris?
#include <strings.h>
#include <sys/wait.h>
#include <unistd.h>

int shell(FILE *restrict stream_in, int scriptMode);
int doCommands(command comLine[], int nrOfCommands);

int main(int argc, char* const argv[]) {
    /* Add signal handler */
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("Couldn't register signal handler");
        exit(1);
    }

    /* Loading init file */
    FILE *scriptfd;
    if ((scriptfd = fopen("~/.mshrc", "r")) != NULL) {
        shell(scriptfd, 1);
    }

    if (argc == 1) {
        return shell(stdin, !isatty(STDIN_FILENO));
    } else if (argc == 2) {
        FILE *scriptfd;
        if ((scriptfd = fopen(argv[1], "r")) == NULL) {
            perror("Scriptfile error:");
            exit(-1);
        }
        return shell(scriptfd, 1);
    } else {
        fprintf(stderr, "Usage: ./msh [scriptfiles...]");
    }
}

/* Run shell, supplied stream will be used as stdin for shell */
int shell(FILE *restrict stream_in, int scriptMode) {
    char line[MAXLINELEN];
    command comLine[MAXCOMMANDS];
    if (!scriptMode) prompt();

    while (fgets(line, MAXLINELEN, stream_in) != NULL) {
        if (line[0] == '\n') {
            if (!scriptMode) prompt();
            continue;
        }

        char eline[MAXLINELEN];
        expand(line, eline);
        extern int nrCommands;
        nrCommands = parse(eline, comLine);

        /* Check for valid commands */
        int valid = 1;
        for (int i = 0; i < nrCommands; i++) {
            if ((comLine[i].argc == 0)
                || ((comLine[i].outfile != NULL) && ((i + 1) < nrCommands))
                || ((comLine[i].infile != NULL) && (i > 0))) {
                valid = 0;
            }
        }
        if (!valid) {
            fprintf(stderr, "Invalid command\n");
            if (!scriptMode) prompt();
            continue;
        }

        /* Custom commands */
        if (strcmp(*comLine->argv, "exit") == 0) {
            exit(0);
        } else if (strcmp(*comLine->argv, "cd") == 0) {
            /* cd - change directory */
            if (chdir(*++comLine->argv) < 0) {
                perror(*comLine->argv);
            }
            if (!scriptMode) prompt();
            continue;
        } else if (strcmp(*comLine->argv, "set") == 0) {
            /* Set environment variable set env=value */
            char *var = strtok(*++(comLine->argv), "=");
            char *value;
            if (var != NULL) {
                value = strtok(NULL, "=");
                if (value != NULL) {
                    setenv(var, value, OVERWRITE_ENVS);
                } else {
                    printf("Usage: set var=value\n");
                }
            }
            if (!scriptMode) prompt();
            continue;
        }

        /* Fork new process and run commands */
        doCommands(comLine, nrCommands);
        if (!scriptMode) {
            if (!scriptMode) prompt();
        }
        nrCommands = 0;
    }
    return 0;
}

/* Do cammands, return status of last command to exit */
int doCommands(command comLine[], int nrOfCommands) {
    /* Pipes for communication between forked child processes */
    int prevPipe[2];
    int currPipe[2];

    /* Will contain pid for every forked process */
    extern int pidArray[];

    /* Fork every command */
    for (int i = 0; i < nrOfCommands; i++) {
        command cmd = comLine[i];

        /* Close pipes that are open, this will be true for every
         * command but the first two. Example:
         * Commnds: ls | sort | more
         *       i:  0 |   1  |   2
         */
        if (i > 1) {
            close(prevPipe[READ_END]);
            close(prevPipe[WRITE_END]);
        }
        /* Save previous pipe in prevPipe */
        if (i > 0) {
            prevPipe[READ_END] = currPipe[READ_END];
            prevPipe[WRITE_END] = currPipe[WRITE_END];
        }

        /* Create a pipe for current command */
        if (pipe(currPipe) != 0 ) {
            perror("Failed to create pipe: ");
            exit(-1);
        }

        /* Create new process */
        switch (pidArray[i] = fork()) {
            case -1: /* Fork failed */
                fprintf(stderr, "fork() failed\n");
                break;

            case 0: /* Child */
                    /* Write output to currPipe[WRITE_END], will be
                     * read by next command for every command but the
                     * last */
                if (i + 1 < nrOfCommands) {
                    if (dupPipe(currPipe, WRITE_END, STDOUT_FILENO ) < 0) {
                        _exit(1);
                    }
                }

                /* Read input from prevPipe[READ_END], created by
                 * previous command every command but the first */
                if (i > 0) {
                    if (dupPipe(prevPipe, READ_END, STDIN_FILENO) < 0) {
                        _exit(-1);
                    }
                }

                if (cmd.infile != NULL) {
                    redirect(cmd.infile, READ_END, STDIN_FILENO);
                }

                if (cmd.outfile != NULL) {
                    redirect(cmd.outfile, WRITE_END, STDOUT_FILENO);
                }

                /* Execute command */
                if (execvp(cmd.argv[0], cmd.argv) < 0 ) {
                    perror(cmd.argv[0]);
                    _exit(1); // Don't flush parents files
                }
                _exit(0); // Shouldn't get here
        }
    }

    // Close pipes
    close(currPipe[READ_END]);
    close(currPipe[WRITE_END]);

    if (nrOfCommands > 1) {
        close(prevPipe[READ_END]);
        close(prevPipe[WRITE_END]);
    }

    /* Wait for every forked process */
    int status;
    for (int i = 0; i < nrOfCommands; i++) {
        while (waitpid(pidArray[i], &status, 0) < 0) {
            if (errno != EINTR) {
                perror("Error waiting for process:");
                break;
            }
        }
        /* printf("Parent wait got status from child: %d, status: %d\n", */
        /*        pidArray[i], status); */
        /* printf("WEXITSTATUS: %d\n",WEXITSTATUS(status)); */
        /* printf("WIFEXITED: %d\n",WIFEXITED(status)); */
        /* printf("WIFSIGNALED: %d\n",WIFSIGNALED(status)); */
        /* printf("WIFSTOPPED: %d\n",WIFSTOPPED(status)); */
    }
    return status; // Return status of last exited child
}

#include "execute.h"
#include "parser.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>

/*
 * int prompt(); Den här funktionen ska helt enkelt bara skriva ut
 * prompten på stdout. Det den ska skriva ut är värdet av
 * miljö-variabeln prompt. Är denna inte satt ska prompten "msh%"
 * användas.
 */

/* Write a prompt on stdout
 * Returns:     0
 */
int prompt() {
    char* prompt;
    fflush(stdout);
    if ((prompt = getenv("PROMPT")) != NULL) {
        printf("%s", prompt);
    } else {
        printf("pid: %d, msh%%", getpid());
    }
    return 0;
}

/*
 * int expand(const char*rawline, char *eline); Syftet med den här
 * funktionen är att expandera vissa specialtecken som man kan skicka
 * till shellet. Det som krävs är att den expanderar miljövariabler.
 * Dessa markeras med ett $ sedan följer namnet på miljö-variablen
 * direkt omslutet av parenteser. Exempel: $(user) Funktionen tar
 * strängen rawline och expanderar den och lägger resultatet i eline.
 * Tänk på att eline har begränsad storlek.
 */

/* Expands special characters.
 * Arguments:   rawline The line that has been read.
 *              eline   The line after special characters have been expanded.
 * Returns:     Length of eline, or -1 if an error occured.
 */
int expand(const char* rawline, char *eline) {
    printf("strlen(eline): %ld\n", strlen(eline));
    int esize = 0;
    eline[0] = '\0';

    for (int i = 0; rawline[i]; i++) {
        if (rawline[i] == '$' && rawline[i + 1] == '(') {
            //int tokenStartIndex = i;
            i = i + 2; // skip '$('
            
            char token[MAXLINELEN];
            int tokenIndex = 0;
            
            while (rawline[i] && rawline[i] != ')') {
                token[tokenIndex++] = rawline[i];
                printf("found env char: %c\n", rawline[i]);
                i++;
            }
            token[tokenIndex] = '\0'; // End token;
            
            // A complete $(text) is found.
            if (rawline[i] == ')') {
                // printf("found env %s: %s\n", token, getenv(token));
                // Get env
                char * env;
                if ((env = getenv(token)) != NULL ) {
                    printf("char:\n");
                    while (*env) {
                        printf("%c", *env);
                        eline[esize++] = *env++;
                    }
                    printf("\n:char:\n");
                    // printf("esize: %d, eline: %s\n", esize, eline);
                }
            } else {
                fprintf(stderr, "environment reference did not end with )\n");
                return -1;
            }
        } else {
            eline[esize++] = rawline[i];
        }
    }
    eline[esize] = '\0'; // Finish string
    printf("\nexpand end, eline: '%s', esize '%d'\n", eline, esize);
    return esize;
}

/*
 * int dupPipe(int pip[2], int end, int destfd); Duplicerar en ände av
 * en pipa till en fildeskriptor, och stänger båda pipändar. Se
 * execute.h för mer information om argument och returvärde för den
 * här funktionen.
 */

/* Duplicate a pipe to a standard I/O file descriptor, and close both
 * pipe ends.
 *
 * Arguments: pip the pipe end tells which end of the pipe
 * shold be dup'ed; it can be one of READ_END or WRITE_END destfd the
 * standard I/O file descriptor to be replaced Returns: -1 on error,
 * else destfd
 */
int dupPipe(int pip[2], int end, int destfd) {
    if (pip[end] == destfd) { // No need to duplicate
        fprintf(stderr, "Don't need to dup, they are the same!\n'");
        return destfd;
    }
    
    if (dup2(pip[end], destfd) < 0) {
        perror("Couldn't dup2 pipe");
        return -1;
    }
    
    close(pip[READ_END]);
    close(pip[WRITE_END]);
    return destfd;
}


/*
 * int redirect(char *filename, int flags, int destfd); Dirigerar om
 * standard-input eller -output till en fil. Inga filer ska skrivas
 * över! Om filen redan finns ska en felkod returneras. Se execute.h
 * för mer information om argument och returvärde för den här
 * funktionen.
 */

/* Redirect a standard I/O file descriptor to a file
 * Arguments:  filename the file to/from which the standard I/O file
 *             descriptor should be redirected
 *       flags indicates whether the file should be opened for reading
 *          or writing
 *       destfd   the standard I/O file descriptor which shall be
 *       redirected
 * Returns: -1 on error, else destfd
 */
int redirect(char *filename, int flags, int destfd) {
    return -1;
}

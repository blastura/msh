#include "execute.h"
#include <stdio.h>

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
    printf("msh$ ");
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
    return -1;
}


/*
 *  int dupPipe(int pip[2], int end, int destfd); Duplicerar en ände av
 * en pipa till en fildeskriptor, och stänger båda pipändar. Se
 * execute.h för mer information om argument och returvärde för den här
 * funktionen.
 */

/* Duplicate a pipe to a standard I/O file descriptor, and close both pipe ends
 * Arguments:  pip   the pipe
 *    end   tells which end of the pipe shold be dup'ed; it can be
 *       one of READ_END or WRITE_END
 *    destfd   the standard I/O file descriptor to be replaced
 * Returns: -1 on error, else destfd
 */
int dupPipe(int pip[2], int end, int destfd) {
    return -1;
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

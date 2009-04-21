#include "execute.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

int getline(char line[], int maxline);

int main(int argc, char* const argv[]) {
    prompt();
    char line[MAXLINELEN];
    int len;
    command comLine[MAXCOMMANDS];
    
    while ((len = getline(line, MAXLINELEN)) > 0) {
        printf("length %d\n", len);
        parse(line, comLine);
        
        printf("argv %s\n", *comLine->argv);
        printf("argc %d\n", comLine->argc);
        printf("infile %s\n", comLine->infile);
        printf("outfile %s\n", comLine->outfile);
        printf("internal %d\n", comLine->internal);
        
        if (strcmp(line, "quit\n") == 0) {
            exit(0);
        } else {
            prompt();
        }
    }
    return 0;
}

/* getline: read a line into s, return length, from
 * http://users.powernet.co.uk/eton/kandr2/krx116.html
 */
int getline(char s[], int lim) {
    int c, i, j;
    for (i = 0, j = 0; (c = getchar()) != EOF && c != '\n'; ++i) {
        if (i < lim - 1) {
            s[j++] = c;
        }
    }
    if (c == '\n') {
        if (i <= lim - 1) {
            s[j++] = c;
        }
        ++i;
    }
    s[j] = '\0';
    return i;
}

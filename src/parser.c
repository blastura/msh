/* parser.c
 * This file contains a parser which parses shell command lines
 * and divides them into commands separated by pipes.
 *
 * Author: Peter Jacobson
 * Date:   950706
 *
 * **********
 * Modified by: Tomas Halvarsson
 * Date:        1997-10-06
 * What?        Added extra syntax checks for redirections,
 *              added extra syntax checks for pipes,
 *              changed some comments
 * Comment:     Compile with -DORIGINAL to get the original functionality
 *
 * Date:        1998-10-04
 * What?        Added extra syntax checks for redirections (for "ls >" etc.)
 * Comment:	Thank heavens for students who test our code thorougly :-)
 * 		(Hey! Check the date! This is becoming like a yearly tradition,
 * 		changing this code.)
 *
 * Date:        1998-12-06
 * What?        Changed some more comments, added some error checks
 *
 * Date:	1999-08-01
 * What?	Reorganized the code
 * **********
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include "parser.h"

static char newline[MAXLINELEN];
static char *words[MAXWORDS];


/* parse() parses a command line with commands separated with pipe (|)
 * symbols 
 * For each command optional input and output redirection files
 * are located.
 * parse() puts the commands in the array comLine and returns
 * the number of commands in the commandline.
 * If a syntax error occured parse() prints an error message and returns 0
 *
 * The commands have the syntax
 * command [args ...] [< path] [> path] | command ...
 *
 * This function assumes that comLine[] is big enough, i.e. declared to contain
 * MAXCOMMANDS commands.
 */
int parse(const char *line, command comLine[])
{
	const char *lp;
	char *nlp;
	int wordc = 0, comc = 0, i;

	/* Split command line in words and put in array newline
	 * Also build array of pointers to words
	 */
	lp = line;
	nlp = newline;
	wordc = 0;
	words[wordc] = nlp;
	while (*lp != '\0') {
#ifndef ORIGINAL
		if (wordc == MAXWORDS-1) {
			fprintf(stderr, "Too many words in command.\n");
			return 0;
		}
#endif
		/* Skip leading whitespace */
		while (isspace((int)*lp))
			lp++;

		if (!*lp)
			break;

		if (strchr("|<>", *lp)) {
			/* Found punctuation character */
			*nlp++ = *lp++;
			*nlp++ = '\0';
			wordc++;
			words[wordc] = nlp;
		} else {
			/* Found a word; copy to delimiter */
			while (!isspace((int)*lp) && !strchr("|<>", *lp))
				*nlp++ = *lp++;

			/* End word */
			*nlp++ = '\0';
			wordc++;
			words[wordc] = nlp;
		}
	}

	/* Reset commands */
	for (i = 0; i < wordc / 2 + 1; i++) {
		comLine[i].argv = NULL;
		comLine[i].argc = 0;
		comLine[i].infile = NULL;
		comLine[i].outfile = NULL;
	}

	words[wordc] = NULL;
	/* Build commands */
	for (comc = 0, i = 0; i < wordc; i++) {
		if (comLine[comc].argc == 0) {
			comLine[comc].argv = words + i;
			comLine[comc].argc++;
		}
#ifndef ORIGINAL
		/* the altered code by Tomas */
		else if ((!strcmp(words[i], "<")) && (i+1 < wordc)) {
			if (strchr("|<>", *words[i+1])) {
				fprintf(stderr, "Missing name for redirect.\n");
				return 0;
			} else {
				words[i] = NULL;
				comLine[comc].infile = words[++i];
			}
		} else if ((!strcmp(words[i], ">")) && (i+1 < wordc)) {
			if (strchr("|<>", *words[i+1])) {
				fprintf(stderr, "Missing name for redirect.\n");
				return 0;
			} else {
				words[i] = NULL;
				comLine[comc].outfile = words[++i];
			}
		} else if (!strcmp(words[i], "|")) {
			if ((i+1 < wordc) && strchr("|<>", *words[i+1])) {
				fprintf(stderr, "Invalid null command.\n");
				return 0;
			} else {
				words[i] = NULL;
				comc++;
			}
		} else if (((!strcmp(words[i], "<")) || 
				(!strcmp(words[i], ">"))) && (i == wordc-1)) {
			fprintf(stderr, "Missing name for redirect.\n");
			return 0;
		}
#else
		/* the original code by Peter */
		else if (!strcmp(words[i], "<") && i + 1 < wordc) {
			words[i] = NULL;
			comLine[comc].infile = words[++i];
		} else if (!strcmp(words[i], ">") && i + 1 < wordc) {
			words[i] = NULL;
			comLine[comc].outfile = words[++i];
		} else if (!strcmp(words[i], "|")) {
			words[i] = NULL;
			comc++;
		}
#endif
		else {
			if (comLine[comc].infile || comLine[comc].outfile) {
				fprintf(stderr, "Extra characters after " 
						"command: %s\n",
						words[i]);
				return 0;
			}
			comLine[comc].argc++;
		}
		if (i == wordc-1)
			comc++;
	}

	return comc;
}

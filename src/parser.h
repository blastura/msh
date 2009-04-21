/* parser.h */

/* Author: Peter Jacobson
 * Date:   ??
 *
 * Modified by: Tomas Halvarsson
 * Date:	1998-12-06
 * What?	Changed some comments
 *
 * Date:	1999-08-01
 * What?	Changed comments a bit more
 */

/* command describes a parsed command.
 * argv is an array of command line arguments including the command name
 * argc is the number of words in the array argv
 * infile is the name of the file from which input should be redirected
 *  (NULL if N/A)
 * outfile is the name of the file to which output should be redirected
 *  (NULL if N/A)
 * internal is a field which is not used by the parser, but which
 *  can be used to indicate that the command is an internal command
 */
typedef struct command_t
{
	char **argv;
	int argc;
	char *infile;
	char *outfile;
	int internal;
} command;

#define MAXWORDS	(1024)
#define MAXCOMMANDS	(MAXWORDS / 2 + 1)
#define MAXLINELEN	MAXWORDS

int parse(const char *line, command comLine[]);


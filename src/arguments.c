/*
 * arguments.c
 *
 * This file is part of genpass.
 *
 * genpass is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * genpass is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with genpass.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $Id$
 * $URL$
 */

#include <stdlib.h>
#include <string.h>
#include "arguments.h"

const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;

extern struct arguments *argumenst;

void foutje(void) 
{
	fprintf(stderr, "Human error in arguments.\n\n");
	fprintf(stderr, "See '--usage' or '--help' for info.\n");
	exit(1);
}

int iscijfer(char *argument) 
{
	int i=0;
	int waar=1;
	while ((i < strlen(argument)) && (waar)) {
		if ((isdigit(argument[i])) ||
		    (argument[i] == '-') ||
		    (argument[i] == '+')) {
			waar = 1;
			i++;
		} else {
			waar = 0;
		}
	}
	return(waar);
}

int readInteger(char *value)
{
	if ( iscijfer(value) ) {
		return atoi(value);
	}
	foutje();
	return(-1);
}

error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
	extern struct arguments *arguments;
	arguments = state->input;

	switch (key)
		{
		case 'n':
			arguments->length = readInteger(arg);
			break;
		case 'u':
			arguments->length = readInteger(arg);
			break;
		case 'l':
			arguments->lower = readInteger(arg);
			break;
		case 'd':
			arguments->digits = readInteger(arg);
			break;
		case 'e':
			arguments->extras = readInteger(arg);
			break;
		case 's':
			arguments->seed = readInteger(arg);
			break;
		case 'f':
			configfile[MAXFILES] = arg;
			break;
		case 'v':
			arguments->verbose++;
			break;
		case ARGP_KEY_END:
			break;
		default:
			return ARGP_ERR_UNKNOWN;
		}
	return 0;
}

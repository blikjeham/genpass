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
#include "arguments.h"

const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;

extern struct arguments *argumenst;

error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
	extern struct arguments *arguments;
	arguments = state->input;

	switch (key)
		{
		case 'n':
			arguments->length = atoi(arg);
			break;
		case 'u':
			arguments->length = atoi(arg);
			break;
		case 'l':
			arguments->lower = atoi(arg);
			break;
		case 'd':
			arguments->digits = atoi(arg);
			break;
		case 'e':
			arguments->extras = atoi(arg);
			break;
		case 's':
			arguments->seed = atoi(arg);
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

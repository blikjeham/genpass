/*
 * arguments.h
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

#ifndef ARGUMENTS_H
#define ARGUMENTS_H 1

#include "config.h"
#include <argp.h>

/* Define the maximum number of config files.
 * Not counting the one given from commandline.
 * The one from commandline is at position MAXFILES in the array.
 */
#define MAXFILES 2
char *configfile[MAXFILES+1];

#ifndef CONFDIR
#define CONFDIR "/usr/local"
#endif /* CONFDIR */

struct arguments {
	int verbose;
	int count;
	int length;
	int upper;
	int lower;
	int digits;
	int extras;
	int seed;
} *arguments;

struct arguments *final;
struct arguments *file_args;

static struct
argp_option options[] = {
	{"count", 'c', "#", 0, "Number of passwords to generate"},
	{"length", 'n', "#", 0, "Length of the password"},
	{"upper", 'u', "#", 0, "Number of uppercase characters"},
	{"lower", 'l', "#", 0, "Number of lowercase characters"},
	{"digits", 'd', "#", 0, "Number of digits"},
	{"extra", 'e', "#", 0,
	 "Number of extra (non-alphanumeric) characters"},
	{"seed", 's', "#", 0, "Use this seed"},
	{"configfile", 'f', "FILE", 0, "Use configuration file FILE"},
	{"verbose", 'v', 0, 0, "Be verbose"},
	{ 0 }
};

void foutje(void);

int iscijfer(char *);

int readInteger(char *);

error_t parse_opt(int key, char *arg, struct argp_state *state);

static struct
argp argp = {options, parse_opt, 0,
	     "genpass -- a random password generator"};

#endif /* ARGUMENTS_H */

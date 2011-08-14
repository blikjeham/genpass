/*
 *    genpass - a random password generator
 *    Copyright (C) 2010 Jabik Postmus and Sjoerd Job Postmus
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $Id: genpass.h,v 1.3 2010/09/30 17:29:18 jabik Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

struct settings
{
	int length;
	int uppercase;
	int lowercase;
	int digits;
	int extras;
	int seed;
};

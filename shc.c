/* shc.c */

/**
 * This software contains the 'Alleged RC4' source code.
 * The original source code was published on the Net by a group of cypherpunks.
 * I picked up a modified version from the news.
 * The copyright notice does not apply to that code.
 */
static const char my_name[] = "shc";
static const char version[] = "Version 3.8.3";
static const char subject[] = "Generic Script Compiler";
static const char cpright[] = "Copyright (c) 1994-2005";
static const struct { const char * f, * s, * e; }
	author = { "Francisco", "Rosales", "<frosal@fi.upm.es>" };

static const char * copying[] = {
"Copying:",
"",
"    This program is free software; you can redistribute it and/or modify",
"    it under the terms of the GNU General Public License as published by",
"    the Free Software Foundation; either version 2 of the License, or",
"    (at your option) any later version.",
"",
"    This program is distributed in the hope that it will be useful,",
"    but WITHOUT ANY WARRANTY; without even the implied warranty of",
"    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the",
"    GNU General Public License for more details.",
"",
"    You should have received a copy of the GNU General Public License",
"    along with this program; if not, write to the Free Software",
"    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.",
"",
"    Report problems and questions to:",
"",
0};

static const char * abstract[] = {
"Abstract:",
"",
"    This tool generates a stripped binary executable version",
"    of the script specified at command line.",
"",
"    Binary version will be saved with a .x extension.",
"",
"    You can specify expiration date [-e] too, after which binary will",
"    refuse to be executed, displaying \"[-m]\" instead.",
"",
"    You can compile whatever interpreted script, but valid [-i], [-x]",
"    and [-l] options must be given.",
"",
0};

static const char usage[] = 
"Usage: shc [-e date] [-m addr] [-i iopt] [-x cmnd] [-l lopt] [-rvDTCAh] -f script";

static const char * help[] = {
"",
"    -e %s  Expiration date in dd/mm/yyyy format [none]",
"    -m %s  Message to display upon expiration [\"Please contact your provider\"]",
"    -f %s  File name of the script to compile",
"    -i %s  Inline option for the shell interpreter i.e: -e",
"    -x %s  eXec command, as a printf format i.e: exec('%s',@ARGV);",
"    -l %s  Last shell option i.e: --",
"    -r     Relax security. Make a redistributable binary",
"    -v     Verbose compilation",
"    -D     Switch ON debug exec calls [OFF]",
"    -T     Allow binary to be traceable [no]",
"    -C     Display license and exit",
"    -A     Display abstract and exit",
"    -h     Display help and exit",
"",
"    Environment variables used:",
"    Name    Default  Usage",
"    CC      cc       C compiler command",
"    CFLAGS  <none>   C compiler flags",
"",
"    Please consult the shc(1) man page.",
"",
0};

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define SIZE 4096

static char * file;
static double date[1];
static char * mail = "Please contact your provider";
static char   rlax[1];
static char * shll;
static char * inlo;
static char * xecc;
static char * lsto;
static char * opts;
static char * text;
static int verbose;
static const char DEBUGEXEC_line[] =
"#define DEBUGEXEC	%d	/* Define as 1 to debug execvp calls */\n";
static int DEBUGEXEC_flag;
static const char TRACEABLE_line[] =
"#define TRACEABLE	%d	/* Define as 1 to enable ptrace the executable */\n";
static int TRACEABLE_flag;

static const char * RTC[] = {
"",
"/* rtc.c */",
"",
"#include <sys/stat.h>",
"#include <sys/types.h>",
"",
"#include <errno.h>",
"#include <stdio.h>",
"#include <stdlib.h>",
"#include <string.h>",
"#include <time.h>",
"#include <unistd.h>",
"",
"/**",
" * 'Alleg

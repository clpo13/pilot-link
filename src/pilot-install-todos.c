/* 
 * install-todolist.c:  Palm ToDo list installer
 *
 * Copyright 1996, Robert A. Kaplan
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include "getopt.h"
#include <stdio.h>
#include <stdlib.h>

#include "pi-source.h"
#include "pi-socket.h"
#include "pi-dlp.h"
#include "pi-todo.h"
#include "pi-header.h"

/* Declare prototypes */
static void display_help(char *progname);
void display_splash(char *progname);
int pilot_connect(char *port);

void install_ToDos(int sd, int db, char *filename);

struct option options[] = {
	{"port",        required_argument, NULL, 'p'},
	{"help",        no_argument,       NULL, 'h'},
        {"version",     no_argument,       NULL, 'v'},   
	{"filename",    required_argument, NULL, 'f'},
	{NULL,          0,                 NULL, 0}
};

static const char *optstring = "p:hvf:";

void install_ToDos(int sd, int db, char *filename)
{
	int 	ToDo_size,
		cLen		= 0,
		i		= 0,
		filelen;
	
        char 	*file_text 	= NULL,
		*cPtr 		= file_text,
		*begPtr 	= cPtr,
		note_text[] 	= "";
	
        unsigned char ToDo_buf[0xffff];
	
        struct 	ToDo todo;
        FILE 	*f;	
				
	f = fopen(filename, "r");
	if (f == NULL) {
		perror("fopen");
		exit(1);
	}

	fseek(f, 0, SEEK_END);
	filelen = ftell(f);
	fseek(f, 0, SEEK_SET);

	file_text = (char *) malloc(filelen + 1);
	if (file_text == NULL) {
		perror("malloc()");
		exit(1);
	}

	fread(file_text, filelen, 1, f);

        cPtr = file_text;
        begPtr = cPtr;
        cLen = 0;
        i = 0;
	while (i < filelen) {
		i++;
		/* printf("c:%c.\n",*cPtr); */
		if (*cPtr == '\n') {
			todo.description = begPtr;
			/* replace CR with terminator */
			*cPtr = '\0';

			todo.priority 	= 4;
			todo.complete 	= 0;
			todo.indefinite = 1;
			/* now = time(0);
			   todo.due = *localtime(&now); */
			todo.note = note_text;
			ToDo_size =
			    pack_ToDo(&todo, ToDo_buf,
				      sizeof(ToDo_buf));
			printf("Description: %s\n", todo.description);

			/* printf("todobuf: %s\n",ToDo_buf);       */
			dlp_WriteRecord(sd, db, 0, 0, 0, ToDo_buf,
					ToDo_size, 0);
			cPtr++;
			begPtr = cPtr;
			cLen = 0;
		} else {
			cLen++;
			cPtr++;
		}
	}
	return;
}

static void display_help(char *progname)
{
	printf("   Updates the Palm ToDo list with entries from a local file\n\n");
	printf("   Usage: %s -p <port> -f <filename>\n", progname);
	printf("   Options:\n");
	printf("   -p, --port <port>         Use device file <port> to communicate with Palm\n");
	printf("   -h, --help                Display help information for %s\n", progname);
	printf("   -v, --version             Display %s version information\n", progname);
	printf("   -f, --filename <filename> A local file with formatted ToDo entries\n\n");
	printf("   Examples: %s -p /dev/pilot -f $HOME/MyTodoList.txt\n\n", progname);
	printf("   The format of this file is a simple line-by-line ToDo task entry.\n");
	printf("   For each new line in the local file, a new task is created in the\n");
	printf("   ToDo database on the Palm.\n\n");

        exit(0);
}

int main(int argc, char *argv[])
{
	int 	c,		/* switch */
		db,
		sd 		= -1;
        char 	*progname 	= argv[0],
		*filename 	= NULL,
		*port 		= NULL;
	struct 	PilotUser User;

	while ((c = getopt_long(argc, argv, optstring, options, NULL)) != -1) {
		switch (c) {

		  case 'h':
			  display_help(progname);
			  exit(0);
		  case 'p':
			  port = optarg;
			  break;
		  case 'f':
			  filename = optarg;
			  break;
                  case 'v':
                          display_splash(progname);
                          exit(0);
		}
	}
	
	if (filename == NULL) {
		printf("\n");
		printf("   ERROR: You must specify a filename to read ToDo entries from.\n");
		printf("   Please see %s --help for more information.\n\n", progname);
		exit(1);
	}

	if (argc < 2 && !getenv("PILOTPORT")) {
		display_splash(progname);
	} else if (port == NULL && getenv("PILOTPORT")) {
		port = getenv("PILOTPORT");
	}

	if (port == NULL && argc > 1) {
		printf("\n");
		printf("   ERROR: At least one command parameter of '-p <port>' must be set, or the\n"
		     "environment variable $PILOTPORT must be used if '-p' is omitted or missing.\n\n");
		exit(1);
	} else if (port != NULL) {
		
		sd = pilot_connect(port);

		/* Did we get a valid socket descriptor back? */
		if (dlp_OpenConduit(sd) < 0) {
			exit(1);
		}
		
		/* Tell user (via Palm) that we are starting things up */
		dlp_OpenConduit(sd);
	
		/* Open the ToDo Pad's database, store access handle in db */
		if (dlp_OpenDB(sd, 0, 0x80 | 0x40, "ToDoDB", &db) < 0) {
			puts("Unable to open ToDoDB");
			dlp_AddSyncLogEntry(sd, "Unable to open ToDoDB.\n");
			exit(1);
		}

		/* Actually do the install here, passed a filename */
		install_ToDos(sd, db, filename);
		
		/* Close the database */
		dlp_CloseDB(sd, db);
	
		/* Tell the user who it is, with a different PC id. */
		User.lastSyncPC 	= 0x00010000;
		User.successfulSyncDate = time(NULL);
		User.lastSyncDate 	= User.successfulSyncDate;
	
		dlp_AddSyncLogEntry(sd, "Wrote ToDo list entries to Palm.\nThank you for using pilot-link.\n");
	
		/* All of the following code is now unnecessary, but harmless */
		dlp_EndOfSync(sd, 0);
		pi_close(sd);
	}
	return 0;
}

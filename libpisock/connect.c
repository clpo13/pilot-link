/*
 * connect.c:  Palm Serial, USB, IR connection routines
 *
 * Copyright (c) 2001, David A. Desrosiers
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include "pi-socket.h"
#include "pi-dlp.h"

/* Declare prototypes */
int pilot_connect(char *port)
{
	int 	sd, 
		result, 
		err	= 0;
	struct 	pi_sockaddr addr;
	struct 	stat attr;			/* Device attributes			*/
	struct  SysInfo sys_info;
	char 	*defport = "/dev/pilot";	/* Default port if none specified 	*/

	if (port == NULL && (port = getenv("PILOTPORT")) == NULL) {
		fprintf(stderr, "No $PILOTPORT specified and no -p <port> given.\n"
			"Defaulting to '%s'\n\n", defport);
		port = defport;
		err = stat(port, &attr);
	}

	if (err) {
		perror("   ERROR");
		fprintf(stderr, "   Error accessing: '%s'. Does '%s' exist?\n",
		       port, port);
		fprintf(stderr, "   Please use --help for more information\n\n");
		exit(1);
	}

	if (!(sd = pi_socket(PI_AF_PILOT, PI_SOCK_STREAM, PI_PF_DLP))) {
		fprintf(stderr, "\n   Unable to create socket '%s'\n",
			port ? port : getenv("PILOTPORT"));
		return -1;
	}

	if (port != NULL) {
		addr.pi_family = PI_AF_PILOT;
		strncpy(addr.pi_device, port, sizeof(addr.pi_device));
		result =
		    pi_bind(sd, (struct sockaddr *) &addr, sizeof(addr));
	} else {
		result = pi_bind(sd, NULL, 0);
	}

	if (result < 0) {
		int save_errno = errno;
		char *portname;

		portname = (port != NULL) ? port : getenv("PILOTPORT");

		if (portname) {
			fprintf(stderr, "\n");
			errno = save_errno;
			perror("   ERROR"); 
			fprintf(stderr, "   Unable to bind to port '%s'\n",
				portname);
	                fprintf(stderr, "   Please use --help for more information\n\n");
		} else
			fprintf(stderr, "\n   No port specified\n");
		pi_close(sd);
		return -1;
	}

	fprintf(stderr,
		"\n   Listening to port: %s\n\n   Please press the HotSync button now... ",
		port ? port : getenv("PILOTPORT"));

	if (pi_listen(sd, 1) == -1) {
		fprintf(stderr, "\n   Error listening on %s\n", port);
		pi_close(sd);
		return -1;
	}

	sd = pi_accept(sd, 0, 0);
	if (sd == -1) {
		fprintf(stderr, "\n   Error accepting data on %s\n", port);
		pi_close(sd);
		return -1;
	}


	fprintf(stderr, "Connected\n\n");


	if (dlp_ReadSysInfo (sd, &sys_info) < 0) {
		fprintf(stderr, "\n   Error read system info on %s\n", port);
		pi_close(sd);
		return -1;
	}

	dlp_OpenConduit(sd);
	return sd;
}

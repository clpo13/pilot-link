/*
 * userland.c: General definitions for userland conduits.
 *
 * Copyright (C) 2004 by Adriaan de Groot <groot@kde.org>
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

#include "userland.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pi-header.h"
#include "pi-socket.h"
#include "pi-dlp.h"


int plu_connect()
{
	int sd = -1;
	int result;

	struct  SysInfo sys_info;

	if ((sd = pi_socket(PI_AF_PILOT,
			PI_SOCK_STREAM, PI_PF_DLP)) < 0) {
		fprintf(stderr, "\n   Unable to create socket '%s'\n", plu_port);
		return -1;
	}

	result = pi_bind(sd, plu_port);

	if (result < 0) {
		fprintf(stderr, "\n   Unable to bind to port: %s\n",
				plu_port);

		fprintf(stderr, "   Please use --help for more "
				"information\n\n");
		return result;
	}

	if (!plu_quiet && isatty(fileno(stdout))) {
		printf("\n   Listening to port: %s\n\n"
			"   Please press the HotSync button now... ",
			plu_port);
	}

	if (pi_listen(sd, 1) < 0) {
		fprintf(stderr, "\n   Error listening on %s\n", plu_port);
		pi_close(sd);
		return -1;
	}

	sd = pi_accept(sd, 0, 0);
	if (sd < 0) {
		fprintf(stderr, "\n   Error accepting data on %s\n", plu_port);
		pi_close(sd);
		return -1;
	}

	if (!plu_quiet && isatty(fileno(stdout))) {
		printf("connected!\n\n");
	}

	if (dlp_ReadSysInfo(sd, &sys_info) < 0) {
		fprintf(stderr, "\n   Error read system info on %s\n", plu_port);
		pi_close(sd);
		return -1;
	}

	dlp_OpenConduit(sd);
	return sd;
}


int plu_findcategory(const struct CategoryAppInfo *info, const char *name)
{
	int index, match_category;

	match_category = -1;
	for (index = 0; index < 16; index += 1) {
		if ((info->name[index][0]) &&
			(strncasecmp(info->name[index], name, 15) == 0)) {
			match_category = index;
			break;
		}
	}

	return match_category;
}


int plu_protect_files(char *name, const char *extension, const size_t namelength)
{
	char *save_name;
	char c=1;

	save_name = strdup( name );
	if (NULL == save_name) {
		fprintf(stderr,"   ERROR: No memory for filename %s%s\n",name,extension);
		return -1;
	}

	/* 4 byes = _%02d and terminating NUL */
	if (strlen(save_name) + strlen(extension) + 4 > namelength) {
		fprintf(stderr,"   ERROR: Buffer for filename too small.\n");
		free(save_name);
		return -1;
	}

	snprintf(name,namelength,"%s%s",save_name,extension);

	while ( access( name, F_OK ) == 0) {
		snprintf( name, namelength, "%s_%02d%s", save_name, c, extension);
		c++;

		if (c >= 100) {
			fprintf(stderr,"   ERROR: Could not generate filename (tried 100 times).\n");
			free(save_name);
			return 0;
		}
	}

	free(save_name);
	return 1;
}




/* vi: set ts=4 sw=4 sts=4 noexpandtab: cin */

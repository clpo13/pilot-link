/*
 * pi-error.h:  definitions for errors returned by the SOCKET, DLP and
 *              FILE layers
 *
 * Copyright (c) 2004, Florent Pillet.
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
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
 *
 * -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
 */
#ifndef _PILOT_ERROR_H_
#define _PILOT_ERROR_H_

/* SOCKET level errors */
#define PI_ERR_SOCK_DISCONNECTED	-200		/* connection has been broken */
#define PI_ERR_SOCK_INVALID			-201		/* invalid protocol stack */
#define PI_ERR_SOCK_TIMEOUT			-202		/* communications timeout (but link not known as broken) */
#define	PI_ERR_SOCK_CANCELED		-203		/* last data transfer was canceled */
#define PI_ERR_SOCK_IO				-204		/* generic I/O error */

/* DLP level error */
#define PI_ERR_DLP_BUFSIZE			-300		/* provided buffer is not big enough to store data */
#define PI_ERR_DLP_PALMOS			-301		/* a non-zero error was returned by the device */
#define PI_ERR_DLP_UNSUPPORTED		-302		/* this DLP call is not supported by the connected handheld */
#define PI_ERR_DLP_SOCKET			-303		/* invalid socket */
#define	PI_ERR_DLP_DATASIZE			-304		/* requested transfer with data block too large (>64k) */

/* FILE level error */
#define PI_ERR_FILE_INVALID			-400		/* invalid prc/pdb/pqa file */

/* GENERIC errors */
#define PI_ERR_GENERIC_MEMORY		-500		/* not enough memory */

/* Macros */
#define IS_SOCK_ERR(error)			((error)<=-200 && (error)>-300)
#define IS_DLP_ERR(error)			((error)<=-300 && (error)>-400)
#define IS_FILE_ERR(error)			((error)<=-400 && (error)>-500)
#define IS_GENERIC_ERR(error)		((error)<=-500 && (error)>-600)

#endif


/* Error codes for DLP functions */
#define	PI_DLP_ERR_DISCONNECTED		-1			/* socket was disconnected */
#define PI_DLP_ERR_STORAGE			-97			/* not enough storage provided to return data (was -2) */
#define PI_DLP_ERR_MEMORY			-98			/* memory allocation failed */
#define PI_DLP_ERR_PALMOS			-99			/* transaction failed (device returned an error) */
#define PI_DLP_ERR_IO				-100		/* I/O error (i.e. a request couldn't be read or written) */
#define PI_DLP_ERR_UNSUPPORTED_CALL	-129		/* attempted transaction is not supported by the device (i.e. device too old) */
#define PI_DLP_ERR_SOCKET			-130		/* invalid socket */
#define	PI_DLP_ERR_DATA_TOO_LARGE	-131		/* requested transfer with data block too large (>64k) */

/*****************************************************************************
 *
 * This program is free software ; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Id: err.h 349 2005-11-22 22:14:55Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#ifndef __ERR_H__
#define __ERR_H__

#define ERR_ID				FOURCC('E','R','R','_')

//----------------------------------------------------------------
// error codes

#define ERR_NONE			0
#define ERR_BUFFER_FULL		-1
#define ERR_OUT_OF_MEMORY	-2
#define ERR_INVALID_DATA	-3
#define ERR_INVALID_PARAM	-4
#define ERR_NOT_SUPPORTED	-5
#define ERR_NEED_MORE_DATA	-6
#define ERR_FILE_NOT_FOUND	-8
#define ERR_END_OF_FILE		-9
#define ERR_DEVICE_ERROR	-10
#define ERR_SYNCED			-11
#define ERR_DATA_NOT_FOUND	-12
#define ERR_MIME_NOT_FOUND	-13
#define ERR_NOT_DIRECTORY	-14
#define ERR_NOT_COMPATIBLE	-15
#define ERR_CONNECT_FAILED	-16
#define ERR_DROPPING		-17
#define ERR_STOPPED			-18
#define ERR_UNAUTHORIZED	-19
#define ERR_LOADING_HEADER	-20
#define ERR_STUB_ERROR		-21
#define ERR_WAIT			-22
#define	ERR_END_DEMUX_PLAY	-23
#define ERR_DEVICE_NOSET	-24

#define TIME_UNKNOWN		-1
// buffer full: data is not processed, retry later
// need more data: data is processed, but need more to continue

#endif


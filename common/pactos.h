/*	$Id: pactos.h,v 1.2 2001/10/01 03:31:42 riq Exp $	*/
/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <rquesada@core-sdi.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */
/**
 * @file pactos.h
 * Maneja los pactos entre los jugadores
 */

#ifndef __TEG_PACTOS_H
#define __TEG_PACTOS_H

enum {
	PACTO_REQUEST,
	PACTO_ESTABLISHED,
	PACTO_ESTABLISHED_LAST_ROUND,
	PACTO_BROKEN,

	PACTO_LAST,
};

typedef struct _pacto {
	LIST_ENTRY next;
	int src;
	int dst;
	int status;
} PACTOS, *PPACTOS;


TEG_STATUS pactos_attack( int src, int dst );
TEG_STATUS pactos_find( int src, int dst, PPACTOS *p );
TEG_STATUS pactos_del( int src, int dst );
TEG_STATUS pactos_flush();
TEG_STATUS pactos_update(int src, int dst, int status );
TEG_STATUS pactos_add(int src, int dst);
TEG_STATUS pactos_init();


#endif /* __TEG_PACTOS_H */

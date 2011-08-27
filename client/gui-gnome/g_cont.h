/*	$Id: g_cont.h,v 1.5 2001/09/03 00:55:40 riq Exp $	*/
/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <rquesada@core-sdi.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License
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

#ifndef __TEG_GUI_GNOME_GCONT_H
#define __TEG_GUI_GNOME_GCONT_H

#include <gnome.h>

struct _G_conts {
	GnomeCanvasGroup *cont_group;
	int x;
	int y;
};

extern struct _G_conts G_conts[];

void G_cont_create( int cont, GnomeCanvasGroup *root );
int G_cont_tot();

#endif /* __TEG_GUI_GNOME_GCONT_H */

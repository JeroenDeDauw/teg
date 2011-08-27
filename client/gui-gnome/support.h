/*	$Id: support.h,v 1.20 2002/09/21 17:56:38 riq Exp $	*/
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

#ifndef __TEG_GUI_GNOME_SUPPORT_H
#define __TEG_GUI_GNOME_SUPPORT_H

void generic_window_set_parent (GtkWidget * dialog, GtkWindow   * parent);
char * load_pixmap_file( char *name );
void teg_dialog( char* title, char* bigtitle, char* data );
GtkWidget* teg_dialog_new( char* title, char* bigtitle );
void dialog_close( GtkWidget *button, gpointer data );
GtkWidget * teg_dialog_gameover( int numjug, int mission );
gchar *translate_to_utf8(const gchar *string );

#endif /* __TEG_GUI_GNOME_SUPPORT_H */

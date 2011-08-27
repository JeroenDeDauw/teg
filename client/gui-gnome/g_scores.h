/*	$Id: g_scores.h,v 1.3 2002/09/09 03:52:06 riq Exp $	*/
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
/**
 * @file g_scores.h
 */
#ifndef __GUI_GNOME_SCORES_H
#define __GUI_GNOME_SCORES_H

/**! view the hiscores */
void gui_scores_view();

/**! place the scores insice an hbox */
void gui_scores_embed( GtkWidget *hbox );

#endif /* __GUI_GNOME_SCORES_H */

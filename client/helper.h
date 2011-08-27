/*	$Id: helper.h,v 1.4 2002/08/31 17:45:57 riq Exp $	*/
/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <riq@corest.com>
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
 * @file aux.h
 */
#ifndef __TEGC_AUX_H
#define __TEGC_AUX_H

TEG_STATUS aux_status( PCPLAYER pj, char *str );
TEG_STATUS aux_countries( int numjug, char *str );
void aux_draw_all_countries();
TEG_STATUS aux_player_init( PCPLAYER pJ, int numjug, char *name, int color );
TEG_STATUS aux_start_error();

/*! parse one score */
TEG_STATUS aux_scores( PSCORES pS, char *str );

#endif /* __TEGC_AUX_H */

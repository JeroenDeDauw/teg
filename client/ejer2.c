/*	$Id: ejer2.c,v 1.10 2007/07/08 13:47:21 nordi Exp $	*/
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
 * @file ejer2.c
 * Contiene algunas funciones auxiliares para el manejo del estado 'ESTADO_EJER2'
 */

#include "client.h"

static int last_country = -1;

/**
 * @fn TEG_STATUS ejer2_out( int country )
 * Pone 2 ejercitos por la tarjeta que saco
 */
TEG_STATUS ejer2_out( int country )
{
	if( g_countries[ country ].numjug == WHOAMI() ) {
		if( ESTADO_ES( PLAYER_STATUS_TARJETA ) ) {
			if( !tarjeta_es_usada( &g_countries[ country ].tarjeta )) {
				tarjeta_usar( &g_countries[ country ].tarjeta );
				last_country = country;
				net_printf(g_game.fd,TOKEN_EJER2"=%d\n",country);
				return TEG_STATUS_SUCCESS;
			} else {
				textmsg( M_ERR,_("Error, the 2 armies where placed before"));
				return TEG_STATUS_ERROR;
			}
		} else {
			textmsg( M_ERR,_("Error, it's not the time to put 2 armies."));
			return TEG_STATUS_ERROR;
		}
	} else {
		textmsg(M_ERR,_("Error, '%s' isn't one of your countries"),g_countries[country].name);
		return TEG_STATUS_ERROR;
	}
}

TEG_STATUS ejer2_restore_from_error()
{
	if( last_country != -1) {
		tarjeta_desusar( &g_countries[ last_country ].tarjeta );
		gui_tarjeta( -1 );
		last_country = -1;
		return TEG_STATUS_SUCCESS;
	}
	return TEG_STATUS_ERROR;
}

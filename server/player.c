/*	$Id: player.c,v 1.7 2006/03/16 21:59:34 nordi Exp $	*/
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
/*
 * Functions that manage the PLAYERS  
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "server.h"
#include "scores.h"
#include "xmlscores.h"

LIST_ENTRY g_list_player;		/**< list of players */


/* given a players' number it returns a pointer the player */
TEG_STATUS player_whois( int numjug, PSPLAYER *pJ)
{
	PLIST_ENTRY l = g_list_player.Flink;
	PSPLAYER pJ_new;

	while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) ) {
		pJ_new = (PSPLAYER) l;
		if( pJ_new->numjug == numjug) {
			*pJ = pJ_new;
			return TEG_STATUS_SUCCESS;
		}
		l = LIST_NEXT(l);
	}
	return TEG_STATUS_PLAYERNOTFOUND;
}

/* finds a player given its name */
TEG_STATUS player_findbyname( char *name, PSPLAYER *pJ)
{
	PLIST_ENTRY l = g_list_player.Flink;
	PSPLAYER pJ_new;

	while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) ) {
		pJ_new = (PSPLAYER) l;
		if( !strncmp( pJ_new->name, name, sizeof(pJ_new->name) )) {
			*pJ = pJ_new;
			return TEG_STATUS_SUCCESS;
		}
		l = LIST_NEXT(l);
	}
	return TEG_STATUS_PLAYERNOTFOUND;
}

/* delete disconnected players */
TEG_STATUS player_delete_discon( PSPLAYER pJ )
{
	PLIST_ENTRY l = (PLIST_ENTRY) pJ;

	if( pJ->estado == PLAYER_STATUS_DESCONECTADO || pJ->fd == -1) {
		if( pJ->color != -1 )
			color_del( pJ->color );
		l = RemoveHeadList( l->Blink );
		free(l);
	}

	return TEG_STATUS_SUCCESS;
}

/* Initialize the player. */
TEG_STATUS player_initplayer( PSPLAYER pJ )
{
	assert( pJ );

	InitializeListHead( &pJ->countries );
	InitializeListHead( &pJ->deals );
	pJ->hizo_canje = FALSE;
	pJ->tot_exchanges = 0;
	pJ->tot_countries = 0;
	pJ->tot_cards = 0;
	pJ->tot_armies = 0;
	pJ->turno_conq = 0;
	pJ->country_src = -1;
	pJ->country_dst = -1;
	pJ->mission = -1;
	pJ->fichasc_armies = 0;
	pJ->fichasc_conts = 0;

	stats_init( &pJ->player_stats );
	return TEG_STATUS_SUCCESS;
}

/* main initialization */
void player_init( void )
{
	InitializeListHead( &g_list_player );
}

/* returns a free number for the player */
TEG_STATUS player_numjug_libre( int *libre)
{
	char jugs[TEG_MAX_PLAYERS];
	int i;
	PLIST_ENTRY l = g_list_player.Flink;
	PSPLAYER pJ;

	assert( libre );

	memset(jugs,0,sizeof(jugs));

	while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) ) {
		pJ = (PSPLAYER) l;
		if( pJ->is_player ) {
			if( pJ->numjug >= 0 && pJ->numjug < TEG_MAX_PLAYERS ) {
				jugs[pJ->numjug] = 1;
			}
		}

		l = LIST_NEXT(l);
	}

	for(i=0;i<TEG_MAX_PLAYERS;i++) {
		if( jugs[i] == 0) {
			*libre = i;
			return TEG_STATUS_SUCCESS;
		}
	}

	/* server is full */
	return TEG_STATUS_ERROR;
}

/* creates a player and initialize it */
PSPLAYER player_ins( PSPLAYER pJ, BOOLEAN esplayer )
{
	int numjug;
	PSPLAYER newJ;

	assert( pJ );

	if( esplayer && player_numjug_libre( &numjug) != TEG_STATUS_SUCCESS )
		return NULL;

	newJ = (PSPLAYER) malloc( sizeof(SPLAYER) );
	if( newJ==NULL)
		return NULL;

	pJ->numjug = -1;
	pJ->color = -1;
	memmove( newJ, pJ, sizeof(SPLAYER));
	player_initplayer( newJ );
	InitializeListHead( &newJ->next );

	newJ->is_player = esplayer;
	newJ->estado = PLAYER_STATUS_CONNECTED;

	if( esplayer ) {
		newJ->numjug = numjug;
		g_game.players++;
	}

	g_game.connections++;
	InsertTailList( &g_list_player, (PLIST_ENTRY) newJ );

	return newJ;
}

/* deletes (flush) all players */
TEG_STATUS player_flush()
{
	PLIST_ENTRY tmp;

	while( !IsListEmpty( &g_list_player ) ) {
		tmp = RemoveHeadList( &g_list_player );
		if( ((PSPLAYER)tmp)->fd > 0 ) {
			fd_remove( ((PSPLAYER)tmp)->fd );
			((PSPLAYER)tmp)->fd = 0;
		}
		con_text_out(M_INF,("Deleting %s\n"),((PSPLAYER)tmp)->name);
		free( tmp );
	}
	g_game.connections = 0;
	g_game.players = 0;
	return TEG_STATUS_SUCCESS;
}

/* Tells if a player is playing */
BOOLEAN player_is_playing( PSPLAYER pJ )
{
	if( ! pJ->is_player )
		return FALSE;

	if( pJ->estado < PLAYER_STATUS_START || pJ->estado >= PLAYER_STATUS_LAST )
		return FALSE;

	return TRUE;
}

/* release the turn, and give it to the next or prev one */
TEG_STATUS player_give_turn_away( PSPLAYER pJ )
{

	PLAYER_STATUS status = pJ->estado;

	/* needed to prevent loops in xxx_next or xxx_prev algorithms */
	pJ->estado = PLAYER_STATUS_GAMEOVER;

	/* si el player tenia el turno, lo tiene que pasar al sig*/
	if( g_game.turno &&  g_game.turno->numjug == pJ->numjug ) {
		if( status <= PLAYER_STATUS_POSTFICHAS ) {
			fichas_next();
		} else if( status <= PLAYER_STATUS_POSTFICHAS2 ) {
			fichas2_next();
		} else if ( status <= PLAYER_STATUS_POSTFICHASC ) {
			fichasc_next();
		} else { /* tenia el turno */
			turno_next();
		}
	}

#if 0
	/* XXX: Dont do this, the last player may skip his turn */

	/* si el player empezo el turno, digo que lo empezo el anterior */
	if( g_game.empieza_turno && g_game.empieza_turno->numjug == pJ->numjug ) {
		turno_2prevplayer( &g_game.empieza_turno );
	}
#endif

	pJ->estado = status;

	return TEG_STATUS_SUCCESS;
}

/* Puts a player in a GAME OVER state */
TEG_STATUS player_del_soft( PSPLAYER pJ )
{
	assert( pJ );

	if( ! player_is_playing(pJ) )
		return TEG_STATUS_ERROR;

	g_game.playing--;

	/* game without players */
	if( JUEGO_EMPEZADO && g_game.playing == 0 ) {

		con_text_out(M_INF,_("Game without players. Initializing another game.\n"));
		game_end(NULL);

	/* game with just one player... the winner */
	} else if( g_game.playing == 1 && JUEGO_EMPEZADO ) {
		PLIST_ENTRY l = g_list_player.Flink;
		PSPLAYER pJ2=NULL;

		while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) ) {
			pJ2 = (PSPLAYER) l;
			if( pJ2->numjug!=pJ->numjug && player_is_playing(pJ2) ) {
				con_text_out(M_INF,_("Game with one player. Player %s(%d) is the winner\n"),pJ2->name,pJ2->numjug);
				pJ2->estado = PLAYER_STATUS_GAMEOVER;
				break;
			}

			pJ2 = NULL;
			l = LIST_NEXT(l);
		}

		game_end(pJ2);

	/* game may continue normally */
	} else {
		player_give_turn_away( pJ );
		pJ->estado = PLAYER_STATUS_GAMEOVER;
	}

/*	player_initplayer( pJ ); */

	return TEG_STATUS_SUCCESS;
}


/* Deletes a player */
TEG_STATUS player_del_hard( PSPLAYER pJ )
{
	PLIST_ENTRY l = (PLIST_ENTRY) pJ;

	assert( pJ );

	g_game.connections--;

	/* close the connection */
	fd_remove(pJ->fd);
	pJ->fd = -1;

	if( pJ->is_player ) {

		con_text_out(M_INF,_("Player %s(%d) quit the game\n"),pJ->name,pJ->numjug);
		netall_printf( TOKEN_EXIT"=%d\n",pJ->numjug );

		if( player_is_playing ( pJ ) ) {


			player_del_soft( pJ );

			g_game.players--;
			pJ->status_before_discon = PLAYER_STATUS_IDLE;
			pJ->estado = PLAYER_STATUS_DESCONECTADO;
			return TEG_STATUS_SUCCESS;

		} else if( pJ->estado == PLAYER_STATUS_GAMEOVER ) {

			g_game.players--;
			pJ->status_before_discon = PLAYER_STATUS_GAMEOVER;
			pJ->estado = PLAYER_STATUS_DESCONECTADO;
			return TEG_STATUS_SUCCESS;
		}

		color_del( pJ->color );
		g_game.players--;

	} else {
		con_text_out(M_INF,_("Observer %s(%d) quit the game\n"),pJ->name,pJ->numjug);
	}

	/* free the player */
	l = RemoveHeadList( l->Blink );
	free( l );

	return TEG_STATUS_SUCCESS;
}

/* given an index of player [0..MAX_PLAYERS] return the numjug of it */
TEG_STATUS player_from_indice( int j, int *real_j )
{
	PLIST_ENTRY l = g_list_player.Flink;
	PSPLAYER pJ;
	int i=0;

	while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) ) {
		pJ = (PSPLAYER) l;
		if( pJ->is_player && pJ->estado>=PLAYER_STATUS_HABILITADO ) {
			if( j == i ) {
				*real_j = pJ->numjug;
				return TEG_STATUS_SUCCESS;
			}
			i++;
		}
		l = LIST_NEXT(l);
	}
	return TEG_STATUS_PLAYERNOTFOUND;
}

/* assigns a country to a player */
TEG_STATUS player_asignarcountry( int numjug, PCOUNTRY p)
{
	PSPLAYER pJ;

	if( player_whois( numjug, &pJ) != TEG_STATUS_SUCCESS )
		return TEG_STATUS_PLAYERNOTFOUND;

	InsertTailList( &pJ->countries, (PLIST_ENTRY) p );
	p->numjug = numjug;
	pJ->tot_countries++;
	pJ->tot_armies++;		/* cada country viene con un ejercito */
	return TEG_STATUS_SUCCESS;
}

/* given a fd, return the player who owns it */
TEG_STATUS player_whoisfd( int fd, PSPLAYER *j)
{
	PLIST_ENTRY l = g_list_player.Flink;
	PSPLAYER i;

	while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) ) {
		i = (PSPLAYER) l;
		if( i->fd == fd) {
			*j = i;
			return TEG_STATUS_SUCCESS;
		}
		l = LIST_NEXT(l);
	}
	return TEG_STATUS_PLAYERNOTFOUND;
}

/* tells if a player is in a given state */
BOOLEAN player_esta_xxx( int fd, int condicion, int strict )
{
	PSPLAYER pJ;

	if( player_whoisfd( fd, &pJ) == TEG_STATUS_SUCCESS ) {
		if(strict)
			return (pJ->estado == condicion );
		else
			return (pJ->estado >= condicion );
	} else
		return FALSE;
}

/* tells if a player is in a given state */
BOOLEAN player_esta_xxx_plus( int fd, int condicion, int strict, PSPLAYER *j )
{
	if( player_whoisfd( fd, j) == TEG_STATUS_SUCCESS ) {
		if(strict)
			return ((*j)->estado == condicion );
		else
			return ((*j)->estado >= condicion );
	} else
		return FALSE;
}

/* quantity of countries per contient that a player has */
TEG_STATUS player_listar_countries( PSPLAYER pJ, int *countries )
{
	PLIST_ENTRY list;
	PCOUNTRY pP;

	assert( pJ );
	assert( countries );


	list = pJ->countries.Flink;

	while( !IsListEmpty( &pJ->countries ) && (list != &pJ->countries ) ) {
		pP = (PCOUNTRY) list;

		countries[ pP->continente ]++;

		list = LIST_NEXT( list );
	}
	return TEG_STATUS_SUCCESS;
}

/* quantity of continents that a player has */
TEG_STATUS player_listar_conts( PSPLAYER pJ, unsigned long *ret )
{
	int countries[CONT_CANT];
	int i;

	assert( pJ );
	assert( ret );


	memset( countries, 0, sizeof(countries) );


	if( player_listar_countries( pJ, countries ) != TEG_STATUS_SUCCESS ) {
		free( countries );
		return TEG_STATUS_ERROR;
	}

	*ret = 0;

	for(i=0;i< CONT_CANT; i++) {
		if( countries[i] == g_conts[i].cant_countries)
			*ret |= 1 << i;
	}

	return TEG_STATUS_SUCCESS;
}



/* Initialize the start turn variables */
TEG_STATUS player_clear_turn( PSPLAYER pJ )
{
	int i;

	assert( pJ );

	pJ->turno_conq = 0;
	pJ->country_src = -1;
	pJ->country_dst = -1;
	pJ->estado =  PLAYER_STATUS_IDLE;

	/* clean all the regroups the player could have done */
	for(i=0;i<COUNTRIES_CANT;i++) {
		if( g_countries[i].numjug == pJ->numjug )
			g_countries[i].ejer_reagrupe = 0;
	}

	return TEG_STATUS_SUCCESS;
}

/* says the quantity of armies a player can place depending in the quantity of countries */
int player_fichasc_cant( PSPLAYER pJ )
{
	assert( pJ );

	if( pJ->tot_countries <= 6 )
		return 3;
	else
		return pJ->tot_countries/2;
}

/* put all the players in a given state */
TEG_STATUS player_all_set_status( PLAYER_STATUS estado )
{
	PLIST_ENTRY l = g_list_player.Flink;
	PSPLAYER pJ;

	while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) ) {
		pJ = (PSPLAYER) l;
		if( pJ->is_player && pJ->estado >= PLAYER_STATUS_GAMEOVER ) {
			pJ->estado = estado;
		}
		l = LIST_NEXT(l);
	}
	return TEG_STATUS_SUCCESS;
}

/* 'map' for player */
TEG_STATUS player_map( jug_map_func func )
{
	PLIST_ENTRY l = g_list_player.Flink;
	PSPLAYER pJ;

	assert(func);

	while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) ) {
		pJ = (PSPLAYER) l;

		/* I dont know what (func)() will do, so next now */
		l = LIST_NEXT(l);

		if( pJ->is_player ) {
			(func)(pJ);
		}
	}
	return TEG_STATUS_SUCCESS;
}

/* Tell if the player lost the game */
BOOLEAN player_is_lost( PSPLAYER pJ )
{
	assert( pJ );

	if( pJ->tot_countries > 0 )
		return 0;

	return 1;
}

/* Put the player in GAMEOVER state */
TEG_STATUS player_poner_perdio( PSPLAYER pJ )
{
	assert( pJ );
	//assert( pJ->is_player == TRUE );

	if( pJ->estado == PLAYER_STATUS_DESCONECTADO ) {
		scores_insert_player( pJ );
		player_delete_discon( pJ );
		return TEG_STATUS_SUCCESS;
	}

	// We get called if a player's last country is conquered. If the player 
	// surrendered before that happened we must NOT decrement the player count again
	if ( player_is_playing(pJ) )
		g_game.playing--;

	pJ->estado = PLAYER_STATUS_GAMEOVER;

#if 0
	if( g_game.empieza_turno && g_game.empieza_turno->numjug == pJ->numjug ) {
		turno_2prevplayer( &g_game.empieza_turno );
	}
#endif

	return TEG_STATUS_SUCCESS;
}


/* Assigns a name to the player that does not conflict with another names */
TEG_STATUS player_fillname( PSPLAYER pJ, char *name )
{
	PSPLAYER pJ_new;
	char new_name [ PLAYERNAME_MAX_LEN ];

	memset(new_name,0,sizeof(new_name));
	strncpy( new_name, name, sizeof(new_name) -1 );

	strip_invalid(new_name);
	if( player_findbyname(new_name,&pJ_new) == TEG_STATUS_SUCCESS && pJ_new->estado != PLAYER_STATUS_DESCONECTADO ) {
		/* that name is already registered, assign a new name dynamically */
		int n = strlen(new_name);
		if( n < sizeof(pJ->name) - 2 ) {
			new_name[n] = '_';
			player_fillname( pJ, new_name );
		} else {
			if( new_name[n-1] < '0' || new_name[n-1] > '9' )
				new_name[n-1]='0';
			else
				new_name[n-1]++;
			player_fillname( pJ, new_name );
		}
	}
	else {
		strncpy( pJ->name, new_name, sizeof(pJ->name)-1);
		pJ->name[ sizeof(pJ->name) -1 ] = 0;
	}

	return TEG_STATUS_SUCCESS;
}

/* return a disconected player with the same name as pJ */
PSPLAYER player_return_disconnected( PSPLAYER pJ )
{
	PLIST_ENTRY l = g_list_player.Flink;
	PSPLAYER pJ_new;

	while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) ) {
		pJ_new = (PSPLAYER) l;

		if( (pJ_new->estado == PLAYER_STATUS_DESCONECTADO) &&
				strcmp( pJ->name, pJ_new->name ) == 0 
				) {
			g_game.players++;
			g_game.playing++;
			g_game.connections++;
			return pJ_new;
		}

		l = LIST_NEXT(l);
	}
	return NULL;
}

/* return true if the player is disconnected */
BOOLEAN player_is_disconnected( PSPLAYER pJ )
{
	PLIST_ENTRY l = g_list_player.Flink;
	PSPLAYER pJ_new;

	while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) ) {
		pJ_new = (PSPLAYER) l;

		if( (pJ_new->estado == PLAYER_STATUS_DESCONECTADO) &&
				strcmp( pJ->name, pJ_new->name ) == 0 
				) {
			return TRUE;
		}

		l = LIST_NEXT(l);
	}
	return FALSE;
}

/* insert all the player but the ones in GAME OVER */
TEG_STATUS player_insert_scores( PSPLAYER pJ )
{
	scores_insert_player( pJ );
	return TEG_STATUS_SUCCESS;
}

/* kick a robot from the game */
TEG_STATUS player_kick_robot( PSPLAYER pJ )
{
	if( ! pJ->human ) {
		TEG_STATUS s;
		s = player_del_hard( pJ );
		con_text_out_wop(M_INF,_("Robot %s was kicked from the game\n"),pJ->name);
		return s;
	}

	return TEG_STATUS_ERROR;
}

/* kick robots when no human is available */
TEG_STATUS player_kick_unparent_robots( void )
{
	if( g_server.kick_unparent_robots )
	{
		int robots=0, humans=0;
		PLIST_ENTRY l = g_list_player.Flink;
		PSPLAYER pJ;

		while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) )
		{
			pJ = (PSPLAYER) l;

			if( pJ->is_player ) {
				if( pJ->human )
					humans++;
				else
					robots++;
			}
			l = LIST_NEXT(l);
		}

		if( robots && ! humans )
		{
			con_text_out_wop(M_INF,_("Kicking unwanted robots...\n"));
			player_map( player_kick_robot );
		}
	}
	return TEG_STATUS_SUCCESS;
}

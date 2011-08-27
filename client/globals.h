/*	$Id: globals.h,v 1.49 2002/08/31 17:45:57 riq Exp $	*/
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
 * @file globals.h
 */
#ifndef __TEGC_GLOBALS_H
#define __TEGC_GLOBALS_H

#include "client.h"
#include "themes.h"


/*
 * Estas son las funciones que deben exportar los modulos (gtk,win,etc)
 * (para hacer un nuevo port, ver gui-null/null.c)
 *
 * NO CAMBIAR EL ORDEN. DO NOT CHANGE THE ORDER
 *
 */

#if 0
enum {
	GUI_INIT,		/**< inicializacion de variables                                 */
	GUI_MAIN,		/**< main loop */
	GUI_DISCONNECT,		/**< me desconecte callback */
	GUI_CONNECTED,		/**< conectado */
	GUI_HABILITADO,		/**< habilitado */
	GUI_START,		/**< empece a jugar callback */
	GUI_STATUS,		/**< status callback */
	GUI_EXIT,		/**< quit callback */
	GUI_TEXTMSG,		/**< mensaje a la pantalla ppal */
	GUI_FICHAS,		/**< poner las 1ra fichas */
	GUI_FICHAS2,		/**< poner las 2das fichas */
	GUI_TURNO,		/**< es mi turno */
	GUI_ATAQUE,		/**< me estan atacando */
	GUI_COUNTRY,		/**< country, due�o y cant. de ejercitos */
	GUI_COUNTRY_SELECT,	/**< selecciona un country */
	GUI_TROPAS,		/**< tropas */
	GUI_TARJETA,		/**< tarjeta que gane */
	GUI_FICHASC,		/**< fichas de contienente y countries */
	GUI_DADOS,		/**< mostrar dados */
	GUI_REAGRUPE,		/**< mostrar ventana de reagrupe */
	GUI_SENSI,		/**< usada para mostrar sensitiviness en los botones */
	GUI_CANJE,		/**< avisa que hiciste bien el canje */
	GUI_LOST,		/**< un player perdio */
	GUI_WINNER,		/**< un player gano */
	GUI_MISSION,		/**< muestra tu mission */
	GUI_TEXTPLAYERMSG,	/**< mensaje de un player */

	GUI_LAST		/**< este no  existe */
};
#endif

extern TEG_STATUS gui_init(int argc, char **argv);
extern TEG_STATUS gui_main(void);
extern TEG_STATUS gui_exit(char *str);
extern TEG_STATUS gui_textmsg(char *aString );
extern TEG_STATUS gui_disconnect(void);
extern TEG_STATUS gui_habilitado(int numjug);
extern TEG_STATUS gui_connected( char *c);
extern TEG_STATUS gui_reconnected();
extern TEG_STATUS gui_status(void);
extern TEG_STATUS gui_start(void);
extern TEG_STATUS gui_fichas(int cant, int conts);
extern TEG_STATUS gui_turn(PCPLAYER pJ);
extern TEG_STATUS gui_country(int p);
extern TEG_STATUS gui_country_select( int country );
extern TEG_STATUS gui_tropas(int src, int dst, int cant);
extern TEG_STATUS gui_tarjeta(int country );
extern TEG_STATUS gui_dados();
extern TEG_STATUS gui_reagrupe( int src, int dst, int cant );
extern TEG_STATUS gui_sensi();
extern TEG_STATUS gui_canje();
extern TEG_STATUS gui_attack( int src, int dst );
extern TEG_STATUS gui_canje();
extern TEG_STATUS gui_lost(int numjug);
extern TEG_STATUS gui_surrender(int numjug);
extern TEG_STATUS gui_winner(int numjug, int mission);
extern TEG_STATUS gui_mission();
extern TEG_STATUS gui_textplayermsg(char *n,int nj, char *m );
extern TEG_STATUS gui_scores(void);

typedef struct _cgame {
	int fd;				/**< mi fd */
	PLAYER_STATUS estado;		/**< estado del game */
	int numjug;			/**< numero de player (yo) */
	int human;			/**< Am I a human ? */
	int observer;			/**< si soy observer o player */
	int playeres;			/**< cantidad de playeres */
	char myname[PLAYERNAME_MAX_LEN];
	int mycolor;			/**< color preferido */
	char sername[SERVER_NAMELEN];	/**< server name */
	int serport;			/**< server port */
	LIST_ENTRY tarjetas_list;	/**< lista de tarjetas */
	int tarjetas_cant;		/**< cantidad de tarjetas */
	int secret_mission;		/**< which secret mission to accomplish */
	int rules;			/**< rules of the game */
	int dados_srccountry;		/**< utimo country que ataco */
	int dados_src[3];		/**< ultimos dados del atacante */
	int dados_dstcountry;		/**< ultimo country que se defendio */
	int dados_dst[3];		/**< ultimos dados del defensor */
	int msg_show;			/**< mascara de los mensajes que hay que mostrar */
	int with_ggz;			/**< si tiene soporte para GGZ */
	int already_connected;		/**< already connected to the server */
	int robot_in_server;		/**< the robot will be launched in the server */
	char theme[THEME_MAX_NAME];	/**< theme que se esta usando */
	int whos_turn;			/**< playernumber who has the turn */
	int who_started_round;		/**< player who started the round */
	int round_number;		/**< round number */
	BOOLEAN	with_secret_mission;	/**< play with secret missions */
	BOOLEAN	with_common_mission;	/**< if playing with secret mission, includes the common mission */
	BOOLEAN	with_fog_of_war;	/**< playing in Fog of War type of game */
} CJUEGO, *PCJUEGO;

/* funciones y variables exportadas */
extern CJUEGO g_game;

#endif /* __TEGC_GLOBALS_H */

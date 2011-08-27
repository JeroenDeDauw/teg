/*	$Id: themes.c,v 1.33 2006/03/12 15:49:33 nordi Exp $	*/
/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2001 Ricardo Quesada
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
 * File that parse the theme stuff
 * File based in the job example from libxml
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <glib.h>

#include "all.h"
#include "themes.h"
#include "globals.h"
#include "common.h"

#ifndef xmlChildrenNode
#define xmlChildrenNode childs
#define xmlRootNode root
#endif

#undef _DEBUG

#define TEG_THEME_VER_MAJOR (1)
#define TEG_THEME_VER_MINOR (5)

static pTheme g_theme = NULL;	/**< Current theme */
static pTInfo g_tinfo = NULL;	/**< info of all current themes */

static pCountry
parseCountry(xmlDocPtr doc, xmlNodePtr cur)
{
	pCountry ret = NULL;

	ret = (pCountry) malloc(sizeof(Country));

	if (ret == NULL) {
		fprintf(stderr,"Out of memory\n");
		return(NULL);
	}
	
	memset(ret, 0, sizeof(*ret));
	
	ret->name = xmlGetProp(cur, (const xmlChar *) "name");
	if (ret->name == NULL) {
		fprintf(stderr, "XML: 'country' has no 'name'\n");
		goto error;
	}

	ret->file = xmlGetProp(cur, (const xmlChar *) "file");
	if (ret->file == NULL) {
		fprintf(stderr, "XML: 'country' has no 'file'\n");
		goto error;
	}

	ret->pos_x = xmlGetProp(cur, (const xmlChar *) "pos_x");
	if (ret->pos_x == NULL) {
		fprintf(stderr, "XML: 'country' has no 'pos_x'\n");
		goto error;
	}

	ret->pos_y = xmlGetProp(cur, (const xmlChar *) "pos_y");
	if (ret->pos_y == NULL) {
		fprintf(stderr, "XML: 'country' has no 'pos_y'\n");
		goto error;
	}

	ret->army_x = xmlGetProp(cur, (const xmlChar *) "army_x");
	if (ret->army_x == NULL) {
		fprintf(stderr, "XML: 'country' has no 'army_x'\n");
		goto error;
	}

	ret->army_y = xmlGetProp(cur, (const xmlChar *) "army_y");
	if (ret->army_y == NULL) {
		fprintf(stderr, "XML: 'country' has no 'army_y'\n");
		goto error;
	}

	cur = xml_get_element_children( cur );

	if ( cur != NULL ) {
		fprintf(stderr,"Wrong type (%s). Nothing was expected\n", cur->name);
		free(ret);
		return(NULL);
	}

	return(ret);

error:
	free(ret);
	return NULL;
}

static pContinent
parseContinent(xmlDocPtr doc, xmlNodePtr cur)
{
	pContinent ret = NULL;
	pCountry curcountry = NULL;

	ret = (pContinent) malloc(sizeof(Continent));

	if (ret == NULL) {
		fprintf(stderr,"Out of memory\n");
		return(NULL);
	}
	
	memset(ret, 0, sizeof(*ret));
	
	ret->name = xmlGetProp(cur, (const xmlChar *) "name");
	if (ret->name == NULL) {
		fprintf(stderr, "XML: 'continent' has no 'name'\n");
		goto error;
	}

	ret->pos_x = xmlGetProp(cur, (const xmlChar *) "pos_x");
	if (ret->pos_x == NULL) {
		fprintf(stderr, "XML: 'continent' has no 'pos_x'\n");
		goto error;
	}

	ret->pos_y = xmlGetProp(cur, (const xmlChar *) "pos_y");
	if (ret->pos_y == NULL) {
		fprintf(stderr, "XML: 'continent' has no 'pos_y'\n");
		goto error;
	}

	cur = xml_get_element_children( cur );
	while( cur != NULL ) {
		/* country */
		if ( !xmlStrcmp(cur->name, (const xmlChar *) "country") ) {
			curcountry = parseCountry( doc, cur );
			if (curcountry != NULL) {
				ret->countries[ret->i_country++] = curcountry;
				if (ret->i_country >= THEME_COUNTRY_MAX)
					break;
			}
		}

		/* else */
		else {
			fprintf(stderr,"Wrong type (%s). country was expected\n", cur->name);
			free(ret);
			return(NULL);
		}

		cur = xml_get_element_next( cur );
	}

	return(ret);
error:
	free(ret);
	return NULL;
}

static pCards
parseCards(xmlDocPtr doc, xmlNodePtr cur)
{
	pCards ret = NULL;

	ret = (pCards) malloc(sizeof(Cards));

	if (ret == NULL) {
		fprintf(stderr,"Out of memory\n");
		return(NULL);
	}
	
	memset(ret, 0, sizeof(*ret));
	
	cur = xml_get_element_children( cur );
	while( cur != NULL ) {
		/* card */
		if ( !xmlStrcmp(cur->name, (const xmlChar *) "card") ) {

			xmlChar * type = xmlGetProp(cur, (const xmlChar *) "type");
			if (type == NULL)
				fprintf(stderr, "Card has no type\n");

			if( strcmp( (char*)type, "jocker" ) == 0 )
				ret->jocker = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			else if( strcmp( (char*)type, "balloon" ) == 0 )
				ret->balloon= xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			else if( strcmp( (char*)type, "cannon" ) == 0 )
				ret->cannon = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			else if( strcmp( (char*)type, "ship" ) == 0 )
				ret->ship = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			else {
				fprintf(stderr, "Unsupported type (%s) in cards\n",(char*)type);
			}
		}
		else if ( !xmlStrcmp(cur->name, (const xmlChar *) "pos_y") )
			ret->pos_y = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);

		/* else */
		else {
			fprintf(stderr,"Wrong name(%s). card was expected\n", cur->name);
		}

		cur = xml_get_element_next( cur );
	}

	return(ret);
}

static pTheme parseTheme(char *filename)
{
	xmlDocPtr doc = NULL;
	pContinent curcontinent = NULL;
	pCards curcards = NULL;
	xmlNodePtr cur;
	pTheme ret = NULL;
	xmlChar *theme_version;

	/*
	 * build an XML tree from a the file;
	 */
	doc = xmlParseFile(filename);
	if (doc == NULL) return(NULL);

	/*
	 * Check the document is of the right kind
	 */
	
	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		fprintf(stderr,"Empty document\n");
		goto error;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) "teg_theme")) {
		fprintf(stderr,"Wrong type. root node != teg_theme\n");
		goto error;
	}

	/* find theme version */
	theme_version = xmlGetProp(cur, (const xmlChar *) "version");
	if (theme_version== NULL) {
		fprintf(stderr, "XML: 'teg_theme' has no 'version'\n");
		goto error;
	}
	/* compare versions */
	{
		int ver_major, ver_minor;
		char *ret;

		ret = strchr( (char*)theme_version, '.' );

		if( ret ) {
			ver_minor = atoi( ret+1);
			*ret = 0;
		} else
			ver_minor = 0;

		ver_major = atoi( (char*)theme_version );

#ifdef _DEBUG
		fprintf( stderr, "major:%d, minor: %d\n", ver_major, ver_minor );
#endif

		if( ver_major != TEG_THEME_VER_MAJOR || ver_minor > TEG_THEME_VER_MINOR)
		{
			fprintf(stderr, "XML: The version of the XML is incompatible.\n . Supported theme version are %d.x where x is <= %d\n . Current theme version is:%d.%d\n"
					,TEG_THEME_VER_MAJOR
					,TEG_THEME_VER_MINOR
					,ver_major
					,ver_minor );
			goto error;
		}
	}


	/*
	 * Allocate the structure to be returned.
	 */
	ret = (pTheme) malloc(sizeof(Theme));
	if (ret == NULL) {
		fprintf(stderr,"Out of memory\n");
		goto error;
	}
	memset(ret, 0, sizeof(*ret));

	/*
	 * Now, walk the tree.
	 */
	cur = xml_get_element_children( cur );
#if 0
	while ( cur && xmlIsBlankNode ( cur ) ) {
		cur = cur -> next;
	}
#endif
	if ( cur == 0 )
		goto error;

	while (cur != NULL) {

		/* author */
		if (!xmlStrcmp(cur->name, (const xmlChar *) "author") ) {
			ret->author = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (ret->author== NULL) {
				fprintf(stderr, "Theme:Author is empty\n");
				goto error;
			}
		}

		/* email */
		else if (!xmlStrcmp(cur->name, (const xmlChar *) "email") ) {
			ret->email = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (ret->email== NULL){
				fprintf(stderr, "Theme:Email is empty\n");
				goto error;
			}
		}


		/* version */
		else if (!xmlStrcmp(cur->name, (const xmlChar *) "version") ) {
			ret->version= xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (ret->version == NULL) {
				fprintf(stderr, "Theme:version is empty\n");
				goto error;
			}
		}


		/* continents */
		else if (!xmlStrcmp(cur->name, (const xmlChar *) "continent") ) {
			curcontinent = parseContinent( doc, cur );
			if (curcontinent != NULL) {
				ret->continents[ret->i_continent++] = curcontinent;
				if (ret->i_continent>= THEME_CONTINENT_MAX)
					break;
			}
		}

		/* cards */
		else if (!xmlStrcmp(cur->name, (const xmlChar *) "cards") ) {
			curcards = parseCards( doc, cur );
			if (curcards != NULL)
				ret->cards = curcards;
		}

		/* board */
		else if (!xmlStrcmp(cur->name, (const xmlChar *) "board") ) {
			ret->board = xmlGetProp(cur, (const xmlChar *) "file");
			if (ret->board == NULL) {
				fprintf(stderr, "XML: 'board' has no name\n");
				goto error;
			}
		}

		/* armies pos */
		else if (!xmlStrcmp(cur->name, (const xmlChar *) "armies_pos") ) {

			ret->armies_x = xmlGetProp(cur, (const xmlChar *) "x");
			if (ret->armies_x== NULL) {
				fprintf(stderr, "XML: 'armies_pos' has no x\n");
				goto error;
			}
			ret->armies_y = xmlGetProp(cur, (const xmlChar *) "y");
			if (ret->armies_y== NULL) {
				fprintf(stderr, "XML: 'armies_pos' has no y\n");
				goto error;
			}

			/* optional */
			ret->armies_background = xmlGetProp(cur, (const xmlChar *) "background");
			ret->armies_dragable = xmlGetProp(cur, (const xmlChar *) "dragable");
		}

		/* dices pos */
		else if (!xmlStrcmp(cur->name, (const xmlChar *) "dices_pos") ) {

			ret->dices_x = xmlGetProp(cur, (const xmlChar *) "x");
			if (ret->dices_x == NULL) {
				fprintf(stderr, "XML: 'dices_pos' has no x\n");
				goto error;
			}
			ret->dices_y = xmlGetProp(cur, (const xmlChar *) "y");
			if (ret->dices_y == NULL) {
				fprintf(stderr, "XML: 'dices_pos' has no y\n");
				goto error;
			}
		}


		/* dices text */
		else if (!xmlStrcmp(cur->name, (const xmlChar *) "dices_text") ) {

			int i;
			char buf[16];

			ret->dices_color = xmlGetProp(cur, (const xmlChar *) "color");
			if (ret->dices_color== NULL) {
				fprintf(stderr, "XML: 'dices_text' has no color\n");
				goto error;
			}

			/* extended dices support for text. It is optional in dices_text */
			for(i=0;i<2;i++) {
				/* x */
				sprintf(buf,"x%d",i+1);
				ret->dices_ext_text_x[i] = xmlGetProp(cur, (const xmlChar *) buf);
				/* y */
				sprintf(buf,"y%d",i+1);
				ret->dices_ext_text_y[i] = xmlGetProp(cur, (const xmlChar *) buf);
			}
		}

		/* extended dices support */
		else if (!xmlStrcmp(cur->name, (const xmlChar *) "dices_ext_pos") ) {
			int i;
			char buf[16];

#ifdef _DEBUG
			fprintf( stderr, "Found extended dices support\n");
#endif
			for(i=0;i<6;i++) {
				/* x */
				sprintf(buf,"x%d",i+1);
				ret->dices_ext_x[i] = xmlGetProp(cur, (const xmlChar *) buf);
				if( ret->dices_ext_x[i] == NULL ) {
					fprintf(stderr, "XML: 'dices_ext_pos' has no '%s'\n",buf);
					goto error;
				}

				/* y */
				sprintf(buf,"y%d",i+1);
				ret->dices_ext_y[i] = xmlGetProp(cur, (const xmlChar *) buf);
				if( ret->dices_ext_y[i] == NULL ) {
					fprintf(stderr, "XML: 'dices_ext_pos' has no '%s'\n",buf);
					goto error;
				}
			}
		}

		/* choose_colors (optional) */
		else if (!xmlStrcmp(cur->name, (const xmlChar *) "choose_colors") ) {

			/* custom */
			ret->choose_colors_custom = xmlGetProp(cur, (const xmlChar *) "custom");

			/* prefix */
			ret->choose_colors_prefix = xmlGetProp(cur, (const xmlChar *) "prefix");
		}

		/* toolbar (optional) */
		else if (!xmlStrcmp(cur->name, (const xmlChar *) "toolbar") ) {

			/* custom */
			ret->toolbar_custom = xmlGetProp(cur, (const xmlChar *) "custom");

			/* name */
			ret->toolbar_name = xmlGetProp(cur, (const xmlChar *) "name");

			/* text_color */
			ret->toolbar_text_color = xmlGetProp(cur, (const xmlChar *) "text_color");

			/* offset left */
			ret->toolbar_offset_left = xmlGetProp(cur, (const xmlChar *) "offset_left");

			/* offset right */
			ret->toolbar_offset_right = xmlGetProp(cur, (const xmlChar *) "offset_right");
		}



		/* screenshot */
		else if (!xmlStrcmp(cur->name, (const xmlChar *) "screenshot") ) {
			ret->screenshot = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (ret->screenshot== NULL) {
				fprintf(stderr, "XML: 'screenshot' is empty\n");
				goto error;
			}
		}

		/* screen */
		else if (!xmlStrcmp(cur->name, (const xmlChar *) "screen") ) {
			ret->screen_size_x = xmlGetProp(cur, (const xmlChar *) "size_x");
			if (ret->screen_size_x == NULL) {
				fprintf(stderr, "XML: 'screen' has no size_x\n");
				goto error;
			}
			ret->screen_size_y = xmlGetProp(cur, (const xmlChar *) "size_y");
			if (ret->screen_size_x == NULL) {
				fprintf(stderr, "XML: 'screen' has no size_y\n");
				goto error;
			}
		}

		/* map */
		else if (!xmlStrcmp(cur->name, (const xmlChar *) "map") ) {
			ret->board = xmlGetProp(cur, (const xmlChar *) "file");
			if (ret->board == NULL) {
				fprintf(stderr, "XML: 'board' has no name\n");
				goto error;
			}
			ret->board_x = xmlGetProp(cur, (const xmlChar *) "offset_x");
			if (ret->board_x == NULL) {
			  ret->board_x = (xmlChar*)"0";
			}
			ret->board_y = xmlGetProp(cur, (const xmlChar *) "offset_y");
			if (ret->board_y == NULL) {
			  ret->board_y = (xmlChar*)"0";
			}
		}
#ifdef XML_WORKS_OK
		/* dices file */
		else  if (!xmlStrcmp(cur->name, (const xmlChar *) "dices_file") ) {
			int i;

			for(i=0;i<DICES_CANT;i++) {
				char name[40];

				memset(name,0,sizeof(name));
				snprintf(name,sizeof(name)-1,"%d",i+1);
				ret->dices_file[i] = xmlGetProp(cur, (const xmlChar *) name);
				if (ret->dices_file[i]== NULL)
					fprintf(stderr, "Dices_file has no %d\n",i);
			}
		}
#endif	/* XML_WORKS_OK */
		else {
			fprintf(stderr,"Wrong type (%s). 'continent', 'board' or 'map', 'screen' and 'dices' were expected\n", cur->name);
			free(ret);
			goto error;
		}

		cur = xml_get_element_next( cur );
	}

	xmlFreeDoc(doc);
	return(ret);

error:
	if( doc )
		xmlFreeDoc(doc);
	if( ret )
		free(ret);
	return(NULL);
}

static TEG_STATUS theme_fill_country_name()
{
	int j,k,l;

	if( !g_theme )
		return TEG_STATUS_ERROR;

	l=0;
	for(j=0;j<g_theme->i_continent;j++) {
		for(k=0;k<g_theme->continents[j]->i_country;k++) {
			g_countries[l++].name = (char*)g_theme->continents[j]->countries[k]->name;
		}
	}

	return TEG_STATUS_SUCCESS;
}

static TEG_STATUS theme_fill_continent_name()
{
	int i;

	if( !g_theme )
		return TEG_STATUS_ERROR;

	for(i=0;i<g_theme->i_continent;i++) {
		g_conts[i].name = (char*)g_theme->continents[i]->name;
	}

	return TEG_STATUS_SUCCESS;
}

/* Loads the 'name' theme */
TEG_STATUS theme_load( char *name)
{
	char filename[512];

	if( g_theme ) theme_unload();

	/* themes/%s/teg_theme.xml */
	memset(filename,0,sizeof(filename));
	snprintf(filename,sizeof(filename)-1,"themes/%s/teg_theme.xml",name);
	g_theme = parseTheme( filename );

	/* ~/.teg/themes/%s/teg_theme.xml */
	if( g_theme == NULL ) {
		memset(filename,0,sizeof(filename));
		snprintf(filename,sizeof(filename)-1,"%s/%s/themes/%s/teg_theme.xml",g_get_home_dir(),TEG_DIRRC,name);
		g_theme = parseTheme( filename );
	}
	
	/* /usr/local/share/teg/themes/%s/teg_theme.xml */
	if( g_theme == NULL ) {
		memset(filename,0,sizeof(filename));
		snprintf(filename,sizeof(filename)-1,"%s/%s/teg_theme.xml",THEMEDIR,name);
		g_theme = parseTheme( filename );
	}

	if( g_theme == NULL ) {
		fprintf(stderr, "Unable to load theme `%s'\n",name);
		return TEG_STATUS_THEMEERROR;
	}

	/* place new name for continents */
	theme_fill_continent_name();

	/* place new name of the countries */
	theme_fill_country_name();

	return TEG_STATUS_SUCCESS;
}

void theme_unload()
{
}

TEG_STATUS theme_giveme_cards(pTCards pC)
{
	if( g_theme == NULL )
		return TEG_STATUS_ERROR;

	pC->jocker= (char*)g_theme->cards->jocker;
	pC->balloon= (char*)g_theme->cards->balloon;
	pC->ship= (char*)g_theme->cards->ship;
	pC->cannon= (char*)g_theme->cards->cannon;
	pC->pos_y = atoi((char*)g_theme->cards->pos_y);

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS theme_giveme_continent(pTContinent pC, int n)
{
	if( g_theme == NULL )
		return TEG_STATUS_ERROR;

	if( n <0 || n >= g_theme->i_continent )
		return TEG_STATUS_ERROR;

	pC->pos_x = atoi( (char*)g_theme->continents[n]->pos_x );
	pC->pos_y = atoi( (char*)g_theme->continents[n]->pos_y );

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS theme_giveme_country(pTCountry pC, int cont, int n)
{
	if( g_theme == NULL )
		return TEG_STATUS_ERROR;

	if( cont <0 || cont >= g_theme->i_continent )
		return TEG_STATUS_ERROR;

	if( n <0 || n >= g_theme->continents[cont]->i_country )
		return TEG_STATUS_ERROR;

	pC->name = (char*)g_theme->continents[cont]->countries[n]->name;
	pC->file = (char*)g_theme->continents[cont]->countries[n]->file;
	pC->pos_x= atoi( (char*)g_theme->continents[cont]->countries[n]->pos_x);
	pC->pos_y= atoi( (char*)g_theme->continents[cont]->countries[n]->pos_y);
	pC->army_x = atoi( (char*)g_theme->continents[cont]->countries[n]->army_x);
	pC->army_y = atoi( (char*)g_theme->continents[cont]->countries[n]->army_y);

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS theme_giveme_theme(pTTheme pT)
{
	int i;

	if( g_theme == NULL )
		return TEG_STATUS_ERROR;

	pT->author= (char*)g_theme->author;
	pT->email = (char*)g_theme->email;
	pT->version = (char*)g_theme->version;
	pT->screen_size_x = my_atoi ( (char*)g_theme->screen_size_x );
	if ( pT->screen_size_x == -1)
	  pT->screen_size_x = 640;
	pT->screen_size_y = my_atoi ( (char*)g_theme->screen_size_y );
	if ( pT->screen_size_y == -1)
	  pT->screen_size_y = 480;
	pT->board = (char*)g_theme->board;
	pT->board_x = my_atoi( (char*)g_theme->board_x );
	pT->board_y = my_atoi( (char*)g_theme->board_y );

	pT->dices_x = my_atoi( (char*)g_theme->dices_x );
	pT->dices_y = my_atoi( (char*)g_theme->dices_y );
	pT->armies_x = atoi( (char*)g_theme->armies_x );
	pT->armies_y = atoi( (char*)g_theme->armies_y );

	pT->armies_background = 1;	/* default */
	if( g_theme->armies_background )
		pT->armies_background = ( strcasecmp((char*)g_theme->armies_background, "true" ) == 0 );

	pT->armies_dragable = 1;	/* default */
	if( g_theme->armies_background )
		pT->armies_dragable = ( strcasecmp((char*)g_theme->armies_background, "true" ) == 0 );

	for(i=0;i<DICES_CANT;i++)
		pT->dices_file[i] = (char*)g_theme->dices_file[i];

	pT->dices_color = (char*)g_theme->dices_color;
	pT->screenshot = (char*)g_theme->screenshot;

	/* optional features */

	/* extended dices support. This is an optional feature. See Draco Theme */
	for(i=0;i<6;i++)
	{
		pT->dices_ext_x[i] = my_atoi( (char*)g_theme->dices_ext_x[i] );
		pT->dices_ext_y[i] = my_atoi( (char*)g_theme->dices_ext_y[i] );
	}

	for(i=0;i<2;i++)
	{
		pT->dices_ext_text_x[i] = my_atoi( (char*)g_theme->dices_ext_text_x[i] );
		pT->dices_ext_text_y[i] = my_atoi( (char*)g_theme->dices_ext_text_y[i] );
	}

	pT->choose_colors_custom = 0;
	pT->choose_colors_prefix = NULL;
	if( g_theme->choose_colors_custom && strcasecmp((char*)g_theme->choose_colors_custom,"true") == 0 )
		pT->choose_colors_custom = 1;
	pT->choose_colors_prefix = (char*)g_theme->choose_colors_prefix;

	pT->toolbar_custom = 0;
	pT->toolbar_name = NULL;
	pT->toolbar_text_color = NULL;
	if( g_theme->toolbar_custom && strcasecmp((char*)g_theme->toolbar_custom,"true") == 0 )
		pT->toolbar_custom = 1;
	pT->toolbar_name = (char*)g_theme->toolbar_name;
	pT->toolbar_text_color = (char*)g_theme->toolbar_text_color;
	pT->toolbar_offset_left = my_atoi( (char*)g_theme->toolbar_offset_left);
	pT->toolbar_offset_right = my_atoi( (char*)g_theme->toolbar_offset_right);

	return TEG_STATUS_SUCCESS;
}

/* builds a list of available themes */
TEG_STATUS theme_enum_themes( pTInfo pTI )
{
	char *dname = "themes";
	char buf[1000];
	DIR *dir;
	struct dirent *e;
	pTInfo pI, pInext;
	char lugares[3][1000];
	int i;

	if( g_tinfo ) {
		*pTI = *g_tinfo;
		return TEG_STATUS_SUCCESS;
	}


	memset(lugares,0,sizeof(lugares));

	/* themes */
	strncpy(lugares[0],dname,sizeof(lugares[0])-1);

	/* ~/.teg/themes */
	snprintf(lugares[1],sizeof(lugares[1])-1,"%s/%s/themes",g_get_home_dir(),TEG_DIRRC);

	/* /usr/local/share/pixmap/teg_pix/themes */
	strncpy(lugares[2], THEMEDIR,sizeof(buf)-1);


	for(i=0;i<3;i++) {
		if( (dir = opendir (lugares[i]))==NULL)
			continue;

		/* scan for directories with file teg_theme.xml */
		while ((e = readdir (dir)) != NULL) {
			FILE *fp;
			snprintf(buf,sizeof(buf)-1,"%s/%s/teg_theme.xml",lugares[i],e->d_name);
			if( (fp = fopen(buf,"r")) ) {
				
				fclose(fp);
				pI = malloc( sizeof(*pI));
				if( pI == NULL )
					return TEG_STATUS_NOMEM;

				memset(pI,0,sizeof(*pI));
				pI->name = strdup(e->d_name);

				/* insert it in the list */
				if( g_tinfo == NULL ) {
					pI->next = NULL;
					g_tinfo = pI;
				} else {
					/* dont insert duplicated */
					int repeated = 0;
					for(pInext=g_tinfo;pInext;pInext = pInext->next ) {
						if( strncmp(pInext->name,pI->name,strlen(pI->name)) == 0 ) {
							repeated = 1;
							break;
						}

					}
					if( !repeated ) {
						pI->next = g_tinfo->next;
						g_tinfo->next = pI;
					} else
						free(pI);
				}
			}
		}

		closedir(dir);
	}

	if( g_tinfo == NULL )
		return TEG_STATUS_ERROR;

	*pTI = *g_tinfo;
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS theme_init()
{
	g_tinfo = NULL;
	return TEG_STATUS_SUCCESS;
}

void theme_free()
{
	pTInfo pI;

	for( pI = g_tinfo; pI != NULL; )  {
		pTInfo pI2;
		if(pI->name)
			free( pI->name );
		pI2 = pI;
		pI = pI->next;
		free(pI2);
	}
}

/* Finds the path for a filename */
char * theme_load_file( char *name )
{
	FILE *fp;
	static char buf[512];

	memset(buf,0,sizeof(buf));

	snprintf(buf,sizeof(buf)-1,"themes/%s/%s",g_game.theme,name);
	fp = fopen(buf,"r");
	if( fp == NULL ) {
		snprintf(buf,sizeof(buf)-1,"%s/%s/themes/%s/%s",g_get_home_dir(),TEG_DIRRC,g_game.theme,name);
		fp = fopen(buf,"r");
	}

	if( fp == NULL ) {
		snprintf(buf,sizeof(buf)-1,"%s/%s/%s",THEMEDIR,g_game.theme,name);
		fp = fopen(buf,"r");
	}

	if( !fp )
		return NULL;

	fclose(fp);
	return buf;
}

/* Loads a pixmap of a not loaded theme */
char * theme_load_fake_file( char *name, char *theme )
{
	FILE *fp;
	static char buf[512];

	memset(buf,0,sizeof(buf));

	snprintf(buf,sizeof(buf)-1,"themes/%s/%s",theme,name);
	fp = fopen(buf,"r");
	if( fp == NULL ) {
		snprintf(buf,sizeof(buf)-1,"%s/%s/themes/%s/%s",g_get_home_dir(),TEG_DIRRC,theme,name);
		fp = fopen(buf,"r");
	}

	if( fp == NULL ) {
		snprintf(buf,sizeof(buf)-1,"%s/%s/%s",THEMEDIR,theme,name);
		fp = fopen(buf,"r");
	}

	if( !fp )
		return NULL;

	fclose(fp);
	return buf;
}

int theme_using_extended_dices()
{
	if( g_theme == NULL ) {
		fprintf(stderr,"theme_using_extended() cant be called before loading a theme!\n");
		return 0;
	}

	return ( my_atoi( (char*)g_theme->dices_ext_x[0] ) != -1 );
}

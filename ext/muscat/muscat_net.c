/*
   +----------------------------------------------------------------------+
   | Muscat/Empower 1.5 Module for PHP version 4.0                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 2000 Ananova Ltd                                       |
   | http://www.ananova.com                                               |
   | Ananova Ltd, PO Box 36, Leeds, LS11 5ZA, United Kingdom              |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU General Public License          |
   | as published by the Free Software Foundation; either version 2       |
   | of the License, or (at your option) any later version.               |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of the GNU General Public License    |
   | along with this program; if not, write to the Free Software          |
   | Foundation, Inc., 59 Temple Place - Suite 330, Boston,               |
   | MA  02111-1307, USA.                                                 |
   |                                                                      |
   | A copy is availble at http://www.gnu.org/copyleft/gpl.txt            |
   +----------------------------------------------------------------------+
   | Author: Sam Liddicott <sam@ananova.com>                              |
   +----------------------------------------------------------------------+
 */


#include "php.h"
#include "php_ini.h"
#include "php_muscat.h"

#if HAVE_MUSCAT
#include "muscat_net.h"

int _net_muscat_close(struct MuscatNet_handle *handle) {
  
}

int _net_muscat_give(struct MuscatNet_handle *handle, char* string) {
  handle->muscat_response.type=' ';
  SOCK_WRITEL(string,strlen(string),handle->socketd);
  SOCK_WRITEL("\n",1,handle->socketd);
}

int _net_muscat_get(struct MuscatNet_handle * handle) {
  char *buf;
  char *end;
  size_t ammount;
  int clen;
  int rlen;

  if (handle->muscat_response.type == 'P') return GIVING;

  // we can use bcom ourselves!!!
  // read in the 6 byte header...
  if (end=php_sock_fgets((char *)&(handle->muscat_response.bcom), 6, handle->socketd)==NULL) {
    ammount=(char *) end - (char *) (&(handle->muscat_response.bcom)) +1;
    return 1; // NEED TO RAISE ERROR HERE  
  }
  ammount=strlen((char *)&(handle->muscat_response.bcom));
  if (handle->muscat_response.bcom[5]!=' ') {
    printf("%s 5 was not space but [%d]\n",&handle->muscat_response.bcom,handle->muscat_response.bcom[5]);
    return 1; // NEED TO RAISE ERROR HERE  
  }
  handle->muscat_response.bcom[5]=0;
  handle->muscat_response.type=handle->muscat_response.bcom[0];
  // now read bcom[1-4] and turn to an int
  clen=atoi(&(handle->muscat_response.bcom[1]))+1;

  // now read length bytes in!
  buf=emalloc(clen+1);
  memset(buf,0,clen+1);

  if (end=php_sock_fgets(buf, clen, handle->socketd)==NULL) {
    ammount=(char *) end - (char *) (&(handle->muscat_response.bcom)) +1;
  }
  handle->muscat_response.p=buf;
  handle->muscat_response.length=strlen(buf);
  return 0;
}

void _net_discard(struct MuscatNet_handle * handle) {
  while(handle->muscat_response.type!='P') {
    if (0!=_net_muscat_get(handle)) break;
  }
}

#endif	/* HAVE_MUSCAT */

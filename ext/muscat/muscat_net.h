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


#ifndef PHP_MUSCAT_NET_H
#define PHP_MUSCAT_NET_H

#if HAVE_MUSCAT

#include "../standard/fsock.h"
struct MuscatNet_handle {
  struct Muscat_response muscat_response;
  zval *socketr;
  int socketd;
  char buffer[1024];
};

int _net_muscat_close(struct MuscatNet_handle *handle);
int _net_muscat_give(struct MuscatNet_handle *handle, char* string);
int _net_muscat_get(struct MuscatNet_handle * handle);
void _net_discard(struct MuscatNet_handle * handle);

#endif

#endif	/* PHP_MUSCAT_NET_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */

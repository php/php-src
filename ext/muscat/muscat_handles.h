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


#ifndef PHP_MUSCAT_HANDLES_H
#define PHP_MUSCAT_HANDLES_H

#if HAVE_MUSCAT
#include <muscat.h>
#include <muscat_net.h>

typedef union _muscat_handles_union {
      struct Muscat_handle muscat_handle;
      struct MuscatNet_handle muscatnet_handle;
      struct Muscat_response muscat_response;
} _muscat_handles;

typedef struct _muscat_handle_struct {
        int open;   
        int net;
      _muscat_handles handles;
} _muscat_handle;

#endif

#endif	/* PHP_MUSCAT_HANDLES_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */

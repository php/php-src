/*  pwd.h - Try to approximate UN*X's getuser...() functions under MS-DOS.
   Copyright (C) 1990 by Thorsten Ohl, td12@ddagsi3.bitnet

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   $Header$
 */

/* This 'implementation' is conjectured from the use of this functions in
   the RCS and BASH distributions.  Of course these functions don't do too
   much useful things under MS-DOS, but using them avoids many "#ifdef
   MSDOS" in ported UN*X code ...  */

struct group {
	char *gr_name;				/* group name */
	char *gr_passwd;			/* group password */
	int gr_gid;					/* group id */
	char **gr_mem;				/* group members */
};

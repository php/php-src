/* private.h: bcmath library header.    	*/
/*
    Copyright (C) 1991, 1992, 1993, 1994, 1997 Free Software Foundation, Inc.
    Copyright (C) 2000 Philip A. Nelson

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.  (COPYING.LIB)

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to:

      The Free Software Foundation, Inc.
      59 Temple Place, Suite 330
      Boston, MA 02111-1307 USA.

    You may contact the author by:
       e-mail:  philnelson@acm.org
      us-mail:  Philip A. Nelson
                Computer Science Department, 9062
                Western Washington University
                Bellingham, WA 98226-9062
       
*************************************************************************/

/* "Private" routines to bcmath. */

/* variables */
#if SANDER_0
extern bc_num _bc_Free_list;
#endif

/* routines */
int _bc_do_compare (bc_num n1, bc_num n2, int use_sign, int ignore_last);
bc_num _bc_do_add (bc_num n1, bc_num n2, int scale_min);
bc_num _bc_do_sub (bc_num n1, bc_num n2, int scale_min);
void _bc_rm_leading_zeros (bc_num num);

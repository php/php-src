/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Modified for PHP by Andi Gutmans <andi@zend.com>                      |
   +----------------------------------------------------------------------+
 */

/* number.h: Arbitrary precision numbers header file. */

/*  This file is part of GNU bc.
    Copyright (C) 1991, 1992, 1993, 1994, 1997 Free Software Foundation, Inc.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License , or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, write to
    the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

    You may contact the author by:
       e-mail:  phil@cs.wwu.edu
      us-mail:  Philip A. Nelson
                Computer Science Department, 9062
                Western Washington University
                Bellingham, WA 98226-9062
       
*************************************************************************/

#ifndef _NUMBER_H
#define _NUMBER_H

typedef enum {PLUS, MINUS} sign;

typedef struct
    {
      sign n_sign;
      int  n_len;	/* The number of digits before the decimal point. */
      int  n_scale;	/* The number of digits after the decimal point. */
      int  n_refs;      /* The number of pointers to this number. */
      char n_value[1];  /* The storage. Not zero char terminated. It is 
      			   allocated with all other fields.  */
    } bc_struct;

typedef bc_struct *bc_num;

/* The base used in storing the numbers in n_value above.
   Currently this MUST be 10. */

#define BASE 10

/*  Some useful macros and constants. */

#define CH_VAL(c)     (c - '0')
#define BCD_CHAR(d)   (d + '0')

#if 0
#ifdef MIN
#undef MIN
#undef MAX
#endif
#define MAX(a,b)      ((a)>(b)?(a):(b))
#define MIN(a,b)      ((a)>(b)?(b):(a))

#endif

#define ODD(a)        ((a)&1)

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

extern void init_numbers (void);
extern void destruct_numbers (void);
extern void str2num (bc_num *num, char *str, int scale);
extern char *num2str (bc_num num);
extern void bc_add ( bc_num n1, bc_num n2, bc_num *result, int scale_min);
extern void bc_sub (bc_num n1, bc_num n2, bc_num *result, int scale_min);
extern void bc_multiply (bc_num n1, bc_num n2, bc_num *prod, int scale);
extern int bc_divide (bc_num n1, bc_num n2, bc_num *quot, int scale);
extern int bc_modulo (bc_num num1, bc_num num2, bc_num *result, int scale);
extern void bc_raise (bc_num num1, bc_num num2, bc_num *result, int scale);
extern int bc_sqrt (bc_num *num, int scale);
extern int bc_compare (bc_num n1, bc_num n2);

extern void free_num (bc_num *num);
extern void init_num (bc_num *num);

#endif

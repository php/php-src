%{
/*
**  Originally written by Steven M. Bellovin <smb@research.att.com> while
**  at the University of North Carolina at Chapel Hill.  Later tweaked by
**  a couple of people on Usenet.  Completely overhauled by Rich $alz
**  <rsalz@bbn.com> and Jim Berets <jberets@bbn.com> in August, 1990.
**
**  This code is in the public domain and has no copyright.
*/

/* $Id$ */

#include "php.h"

#ifdef PHP_WIN32
#include <malloc.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <ctype.h>

#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif
#ifdef PHP_WIN32
# include "win32/time.h"
#endif

#include "php_parsedate.h"

#if HAVE_STDLIB_H
# include <stdlib.h> /* for `free'; used by Bison 1.27 */
#endif

#if defined(_HPUX_SOURCE)
#include <alloca.h>
#endif

#if defined (STDC_HEADERS) || (!defined (isascii) && !defined (HAVE_ISASCII))
# define IN_CTYPE_DOMAIN(c) 1
#else
# define IN_CTYPE_DOMAIN(c) isascii(c)
#endif

#define ISSPACE(c) (IN_CTYPE_DOMAIN (c) && isspace (c))
#define ISALPHA(c) (IN_CTYPE_DOMAIN (c) && isalpha (c))
#define ISUPPER(c) (IN_CTYPE_DOMAIN (c) && isupper (c))
#define ISDIGIT_LOCALE(c) (IN_CTYPE_DOMAIN (c) && isdigit (c))

/* ISDIGIT differs from ISDIGIT_LOCALE, as follows:
   - Its arg may be any int or unsigned int; it need not be an unsigned char.
   - It's guaranteed to evaluate its argument exactly once.
   - It's typically faster.
   Posix 1003.2-1992 section 2.5.2.1 page 50 lines 1556-1558 says that
   only '0' through '9' are digits.  Prefer ISDIGIT to ISDIGIT_LOCALE unless
   it's important to use the locale's definition of `digit' even when the
   host does not conform to Posix.  */
#define ISDIGIT(c) ((unsigned) (c) - '0' <= 9)

#if defined (STDC_HEADERS) || defined (USG)
# include <string.h>
#endif

#if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 7)
# define __attribute__(x)
#endif

#ifndef ATTRIBUTE_UNUSED
# define ATTRIBUTE_UNUSED __attribute__ ((__unused__))
#endif

/* Some old versions of bison generate parsers that use bcopy.
   That loses on systems that don't provide the function, so we have
   to redefine it here.  */
#if !defined (HAVE_BCOPY) && defined (HAVE_MEMCPY) && !defined (bcopy)
# define bcopy(from, to, len) memcpy ((to), (from), (len))
#endif

/* Remap normal yacc parser interface names (yyparse, yylex, yyerror, etc),
   as well as gratuitiously global symbol names, so we can have multiple
   yacc generated parsers in the same program.  Note that these are only
   the variables produced by yacc.  If other parser generators (bison,
   byacc, etc) produce additional global names that conflict at link time,
   then those parser generators need to be fixed instead of adding those
   names to this list. */

#define yyparse php_gd_parse
#define yylex   php_gd_lex

static int yyerror ();

#define EPOCH		1970
#define HOUR(x)		((x) * 60)

#define MAX_BUFF_LEN    128   /* size of buffer to read the date into */

/*
**  An entry in the lexical lookup table.
*/
typedef struct _TABLE {
    const char	*name;
    int		type;
    int		value;
} TABLE;


/*
**  Meridian:  am, pm, or 24-hour style.
*/
typedef enum _MERIDIAN {
    MERam, MERpm, MER24
} MERIDIAN;

struct date_yy {
	const char	*yyInput;
	int	yyDayOrdinal;
	int	yyDayNumber;
	int	yyHaveDate;
	int	yyHaveDay;
	int	yyHaveRel;
	int	yyHaveTime;
	int	yyHaveZone;
	int	yyTimezone;
	int	yyDay;
	int	yyHour;
	int	yyMinutes;
	int	yyMonth;
	int	yySeconds;
	int	yyYear;
	MERIDIAN	yyMeridian;
	int	yyRelDay;
	int	yyRelHour;
	int	yyRelMinutes;
	int	yyRelMonth;
	int	yyRelSeconds;
	int	yyRelYear;
};

typedef union _date_ll {
    int			Number;
    enum _MERIDIAN	Meridian;
} date_ll;

#define YYPARSE_PARAM parm
#define YYLEX_PARAM parm
#define YYSTYPE date_ll
#define YYLTYPE void

%}

/* This grammar has 14 shift/reduce conflicts. */
%expect 14
%pure_parser

%token	tAGO tDAY tDAY_UNIT tDAYZONE tDST tHOUR_UNIT tID
%token	tMERIDIAN tMINUTE_UNIT tMONTH tMONTH_UNIT
%token	tSEC_UNIT tSNUMBER tUNUMBER tYEAR_UNIT tZONE

%type	<Number>	tDAY tDAY_UNIT tDAYZONE tHOUR_UNIT tMINUTE_UNIT
%type	<Number>	tMONTH tMONTH_UNIT
%type	<Number>	tSEC_UNIT tSNUMBER tUNUMBER tYEAR_UNIT tZONE
%type	<Meridian>	tMERIDIAN o_merid

%%

spec	: /* NULL */
	| spec item
	;

item	: time {
	    ((struct date_yy *)parm)->yyHaveTime++;
	}
	| zone {
	    ((struct date_yy *)parm)->yyHaveZone++;
	}
	| date {
	    ((struct date_yy *)parm)->yyHaveDate++;
	}
	| day {
	    ((struct date_yy *)parm)->yyHaveDay++;
	}
	| rel {
	    ((struct date_yy *)parm)->yyHaveRel++;
	}
	| number
	;

time	: tUNUMBER tMERIDIAN {
	    ((struct date_yy *)parm)->yyHour = $1;
	    ((struct date_yy *)parm)->yyMinutes = 0;
	    ((struct date_yy *)parm)->yySeconds = 0;
	    ((struct date_yy *)parm)->yyMeridian = $2;
	}
	| tUNUMBER ':' tUNUMBER o_merid {
	    ((struct date_yy *)parm)->yyHour = $1;
	    ((struct date_yy *)parm)->yyMinutes = $3;
	    ((struct date_yy *)parm)->yySeconds = 0;
	    ((struct date_yy *)parm)->yyMeridian = $4;
	}
	| tUNUMBER ':' tUNUMBER tSNUMBER {
	    ((struct date_yy *)parm)->yyHour = $1;
	    ((struct date_yy *)parm)->yyMinutes = $3;
	    ((struct date_yy *)parm)->yyMeridian = MER24;
	    ((struct date_yy *)parm)->yyHaveZone++;
	    ((struct date_yy *)parm)->yyTimezone = ($4 < 0
			  ? -$4 % 100 + (-$4 / 100) * 60
			  : - ($4 % 100 + ($4 / 100) * 60));
	}
	| tUNUMBER ':' tUNUMBER ':' tUNUMBER o_merid {
	    ((struct date_yy *)parm)->yyHour = $1;
	    ((struct date_yy *)parm)->yyMinutes = $3;
	    ((struct date_yy *)parm)->yySeconds = $5;
	    ((struct date_yy *)parm)->yyMeridian = $6;
	}
	| tUNUMBER ':' tUNUMBER ':' tUNUMBER tSNUMBER {
	    /* ISO 8601 format.  hh:mm:ss[+-][0-9]{2}([0-9]{2})?.  */
	    ((struct date_yy *)parm)->yyHour = $1;
	    ((struct date_yy *)parm)->yyMinutes = $3;
	    ((struct date_yy *)parm)->yySeconds = $5;
	    ((struct date_yy *)parm)->yyMeridian = MER24;
	    ((struct date_yy *)parm)->yyHaveZone++;
		if ($6 <= -100 || $6 >= 100) {
			((struct date_yy *)parm)->yyTimezone =  
				-$6 % 100 + (-$6 / 100) * 60;
		} else {
			((struct date_yy *)parm)->yyTimezone =  -$6 * 60;
		}
	}
	;

zone	: tZONE {
	    ((struct date_yy *)parm)->yyTimezone = $1;
	}
	| tDAYZONE {
	    ((struct date_yy *)parm)->yyTimezone = $1 - 60;
	}
	|
	  tZONE tDST {
	    ((struct date_yy *)parm)->yyTimezone = $1 - 60;
	}
	;

day	: tDAY {
	    ((struct date_yy *)parm)->yyDayOrdinal = 1;
	    ((struct date_yy *)parm)->yyDayNumber = $1;
	}
	| tDAY ',' {
	    ((struct date_yy *)parm)->yyDayOrdinal = 1;
	    ((struct date_yy *)parm)->yyDayNumber = $1;
	}
	| tUNUMBER tDAY {
	    ((struct date_yy *)parm)->yyDayOrdinal = $1;
	    ((struct date_yy *)parm)->yyDayNumber = $2;
	}
	;

date	: tUNUMBER '/' tUNUMBER {
	    ((struct date_yy *)parm)->yyMonth = $1;
	    ((struct date_yy *)parm)->yyDay = $3;
	}
	| tUNUMBER '/' tUNUMBER '/' tUNUMBER {
	  /* Interpret as YYYY/MM/DD if $1 >= 1000, otherwise as MM/DD/YY.
	     The goal in recognizing YYYY/MM/DD is solely to support legacy
	     machine-generated dates like those in an RCS log listing.  If
	     you want portability, use the ISO 8601 format.  */
	  if ($1 >= 1000)
	    {
	      ((struct date_yy *)parm)->yyYear = $1;
	      ((struct date_yy *)parm)->yyMonth = $3;
	      ((struct date_yy *)parm)->yyDay = $5;
	    }
	  else
	    {
	      ((struct date_yy *)parm)->yyMonth = $1;
	      ((struct date_yy *)parm)->yyDay = $3;
	      ((struct date_yy *)parm)->yyYear = $5;
	    }
	}
	| tUNUMBER tSNUMBER tSNUMBER {
	    /* ISO 8601 format.  yyyy-mm-dd.  */
	    ((struct date_yy *)parm)->yyYear = $1;
	    ((struct date_yy *)parm)->yyMonth = -$2;
	    ((struct date_yy *)parm)->yyDay = -$3;
	}
	| tUNUMBER tMONTH tSNUMBER {
	    /* e.g. 17-JUN-1992.  */
	    ((struct date_yy *)parm)->yyDay = $1;
	    ((struct date_yy *)parm)->yyMonth = $2;
	    ((struct date_yy *)parm)->yyYear = -$3;
	}
	| tMONTH tUNUMBER tUNUMBER {
	    ((struct date_yy *)parm)->yyMonth = $1;
	    ((struct date_yy *)parm)->yyDay = $2;
		((struct date_yy *)parm)->yyYear = $3;
	}
	| tMONTH tUNUMBER {
	    ((struct date_yy *)parm)->yyMonth = $1;
	    ((struct date_yy *)parm)->yyDay = $2;
	}
	| tMONTH tUNUMBER ',' tUNUMBER {
	    ((struct date_yy *)parm)->yyMonth = $1;
	    ((struct date_yy *)parm)->yyDay = $2;
	    ((struct date_yy *)parm)->yyYear = $4;
	}
	| tUNUMBER tMONTH {
	    ((struct date_yy *)parm)->yyMonth = $2;
	    ((struct date_yy *)parm)->yyDay = $1;
	}
	| tUNUMBER tMONTH tUNUMBER {
	    ((struct date_yy *)parm)->yyMonth = $2;
	    ((struct date_yy *)parm)->yyDay = $1;
	    ((struct date_yy *)parm)->yyYear = $3;
	}
	;

rel	: relunit tAGO {
	    ((struct date_yy *)parm)->yyRelSeconds =
			-((struct date_yy *)parm)->yyRelSeconds;
	    ((struct date_yy *)parm)->yyRelMinutes =
			-((struct date_yy *)parm)->yyRelMinutes;
	    ((struct date_yy *)parm)->yyRelHour =
			-((struct date_yy *)parm)->yyRelHour;
	    ((struct date_yy *)parm)->yyRelDay =
			-((struct date_yy *)parm)->yyRelDay;
	    ((struct date_yy *)parm)->yyRelMonth =
			-((struct date_yy *)parm)->yyRelMonth;
	    ((struct date_yy *)parm)->yyRelYear =
			-((struct date_yy *)parm)->yyRelYear;
	}
	| relunit
	;

relunit	: tUNUMBER tYEAR_UNIT {
	    ((struct date_yy *)parm)->yyRelYear += $1 * $2;
	}
	| tSNUMBER tYEAR_UNIT {
	    ((struct date_yy *)parm)->yyRelYear += $1 * $2;
	}
	| tYEAR_UNIT {
	    ((struct date_yy *)parm)->yyRelYear += $1;
	}
	| tUNUMBER tMONTH_UNIT {
	    ((struct date_yy *)parm)->yyRelMonth += $1 * $2;
	}
	| tSNUMBER tMONTH_UNIT {
	    ((struct date_yy *)parm)->yyRelMonth += $1 * $2;
	}
	| tMONTH_UNIT {
	    ((struct date_yy *)parm)->yyRelMonth += $1;
	}
	| tUNUMBER tDAY_UNIT {
	    ((struct date_yy *)parm)->yyRelDay += $1 * $2;
	}
	| tSNUMBER tDAY_UNIT {
	    ((struct date_yy *)parm)->yyRelDay += $1 * $2;
	}
	| tDAY_UNIT {
	    ((struct date_yy *)parm)->yyRelDay += $1;
	}
	| tUNUMBER tHOUR_UNIT {
	    ((struct date_yy *)parm)->yyRelHour += $1 * $2;
	}
	| tSNUMBER tHOUR_UNIT {
	    ((struct date_yy *)parm)->yyRelHour += $1 * $2;
	}
	| tHOUR_UNIT {
	    ((struct date_yy *)parm)->yyRelHour += $1;
	}
	| tUNUMBER tMINUTE_UNIT {
	    ((struct date_yy *)parm)->yyRelMinutes += $1 * $2;
	}
	| tSNUMBER tMINUTE_UNIT {
	    ((struct date_yy *)parm)->yyRelMinutes += $1 * $2;
	}
	| tMINUTE_UNIT {
	    ((struct date_yy *)parm)->yyRelMinutes += $1;
	}
	| tUNUMBER tSEC_UNIT {
	    ((struct date_yy *)parm)->yyRelSeconds += $1 * $2;
	}
	| tSNUMBER tSEC_UNIT {
	    ((struct date_yy *)parm)->yyRelSeconds += $1 * $2;
	}
	| tSEC_UNIT {
	    ((struct date_yy *)parm)->yyRelSeconds += $1;
	}
	;

number	: tUNUMBER
          {
	    if (((struct date_yy *)parm)->yyHaveTime && 
			((struct date_yy *)parm)->yyHaveDate && 
			!((struct date_yy *)parm)->yyHaveRel)
	      ((struct date_yy *)parm)->yyYear = $1;
	    else
	      {
		if ($1>10000)
		  {
		    ((struct date_yy *)parm)->yyHaveDate++;
		    ((struct date_yy *)parm)->yyDay= ($1)%100;
		    ((struct date_yy *)parm)->yyMonth= ($1/100)%100;
		    ((struct date_yy *)parm)->yyYear = $1/10000;
		  }
		else
		  {
		    ((struct date_yy *)parm)->yyHaveTime++;
		    if ($1 < 100)
		      {
			((struct date_yy *)parm)->yyHour = $1;
			((struct date_yy *)parm)->yyMinutes = 0;
		      }
		    else
		      {
		    	((struct date_yy *)parm)->yyHour = $1 / 100;
		    	((struct date_yy *)parm)->yyMinutes = $1 % 100;
		      }
		    ((struct date_yy *)parm)->yySeconds = 0;
		    ((struct date_yy *)parm)->yyMeridian = MER24;
		  }
	      }
	  }
	;

o_merid	: /* NULL */
	  {
	    $$ = MER24;
	  }
	| tMERIDIAN
	  {
	    $$ = $1;
	  }
	;

%%

time_t get_date (char *p, time_t *now);

#ifndef PHP_WIN32
extern struct tm	*gmtime();
extern struct tm	*localtime();
extern time_t		mktime();
#endif

/* Month and day table. */
static TABLE const MonthDayTable[] = {
    { "january",	tMONTH,  1 },
    { "february",	tMONTH,  2 },
    { "march",		tMONTH,  3 },
    { "april",		tMONTH,  4 },
    { "may",		tMONTH,  5 },
    { "june",		tMONTH,  6 },
    { "july",		tMONTH,  7 },
    { "august",		tMONTH,  8 },
    { "september",	tMONTH,  9 },
    { "sept",		tMONTH,  9 },
    { "october",	tMONTH, 10 },
    { "november",	tMONTH, 11 },
    { "december",	tMONTH, 12 },
    { "sunday",		tDAY, 0 },
    { "monday",		tDAY, 1 },
    { "tuesday",	tDAY, 2 },
    { "tues",		tDAY, 2 },
    { "wednesday",	tDAY, 3 },
    { "wednes",		tDAY, 3 },
    { "thursday",	tDAY, 4 },
    { "thur",		tDAY, 4 },
    { "thurs",		tDAY, 4 },
    { "friday",		tDAY, 5 },
    { "saturday",	tDAY, 6 },
    { NULL, 0, 0 }
};

/* Time units table. */
static TABLE const UnitsTable[] = {
    { "year",		tYEAR_UNIT,	1 },
    { "month",		tMONTH_UNIT,	1 },
    { "fortnight",	tDAY_UNIT,	14 },
    { "week",		tDAY_UNIT,	7 },
    { "day",		tDAY_UNIT,	1 },
    { "hour",		tHOUR_UNIT,	1 },
    { "minute",		tMINUTE_UNIT,	1 },
    { "min",		tMINUTE_UNIT,	1 },
    { "second",		tSEC_UNIT,	1 },
    { "sec",		tSEC_UNIT,	1 },
    { NULL, 0, 0 }
};

/* Assorted relative-time words. */
static TABLE const OtherTable[] = {
    { "tomorrow",	tDAY_UNIT,	1 },
    { "yesterday",	tDAY_UNIT,	-1 },
    { "today",		tDAY_UNIT,	0 },
    { "now",		tDAY_UNIT,	0 },
    { "last",		tUNUMBER,	-1 },
    { "this",		tMINUTE_UNIT,	0 },
    { "next",		tUNUMBER,	2 },
    { "first",		tUNUMBER,	1 },
/*  { "second",		tUNUMBER,	2 }, */
    { "third",		tUNUMBER,	3 },
    { "fourth",		tUNUMBER,	4 },
    { "fifth",		tUNUMBER,	5 },
    { "sixth",		tUNUMBER,	6 },
    { "seventh",	tUNUMBER,	7 },
    { "eighth",		tUNUMBER,	8 },
    { "ninth",		tUNUMBER,	9 },
    { "tenth",		tUNUMBER,	10 },
    { "eleventh",	tUNUMBER,	11 },
    { "twelfth",	tUNUMBER,	12 },
    { "ago",		tAGO,	1 },
    { NULL, 0, 0 }
};

/* The timezone table. */
static TABLE const TimezoneTable[] = {
    { "gmt",	tZONE,     HOUR ( 0) },	/* Greenwich Mean */
    { "ut",	tZONE,     HOUR ( 0) },	/* Universal (Coordinated) */
    { "utc",	tZONE,     HOUR ( 0) },
    { "wet",	tZONE,     HOUR ( 0) },	/* Western European */
    { "bst",	tDAYZONE,  HOUR ( 0) },	/* British Summer */
    { "wat",	tZONE,     HOUR ( 1) },	/* West Africa */
    { "at",	tZONE,     HOUR ( 2) },	/* Azores */
#if	0
    /* For completeness.  BST is also British Summer, and GST is
     * also Guam Standard. */
    { "bst",	tZONE,     HOUR ( 3) },	/* Brazil Standard */
    { "gst",	tZONE,     HOUR ( 3) },	/* Greenland Standard */
#endif
#if 0
    { "nft",	tZONE,     HOUR (3.5) },	/* Newfoundland */
    { "nst",	tZONE,     HOUR (3.5) },	/* Newfoundland Standard */
    { "ndt",	tDAYZONE,  HOUR (3.5) },	/* Newfoundland Daylight */
#endif
    { "ast",	tZONE,     HOUR ( 4) },	/* Atlantic Standard */
    { "adt",	tDAYZONE,  HOUR ( 4) },	/* Atlantic Daylight */
    { "est",	tZONE,     HOUR ( 5) },	/* Eastern Standard */
    { "edt",	tDAYZONE,  HOUR ( 5) },	/* Eastern Daylight */
    { "cst",	tZONE,     HOUR ( 6) },	/* Central Standard */
    { "cdt",	tDAYZONE,  HOUR ( 6) },	/* Central Daylight */
    { "mst",	tZONE,     HOUR ( 7) },	/* Mountain Standard */
    { "mdt",	tDAYZONE,  HOUR ( 7) },	/* Mountain Daylight */
    { "pst",	tZONE,     HOUR ( 8) },	/* Pacific Standard */
    { "pdt",	tDAYZONE,  HOUR ( 8) },	/* Pacific Daylight */
    { "yst",	tZONE,     HOUR ( 9) },	/* Yukon Standard */
    { "ydt",	tDAYZONE,  HOUR ( 9) },	/* Yukon Daylight */
    { "hst",	tZONE,     HOUR (10) },	/* Hawaii Standard */
    { "hdt",	tDAYZONE,  HOUR (10) },	/* Hawaii Daylight */
    { "cat",	tZONE,     HOUR (10) },	/* Central Alaska */
    { "akst",	tZONE,     HOUR (10) }, /* Alaska Standard */
    { "akdt",	tZONE,     HOUR (10) }, /* Alaska Daylight */
    { "ahst",	tZONE,     HOUR (10) },	/* Alaska-Hawaii Standard */
    { "nt",	tZONE,     HOUR (11) },	/* Nome */
    { "idlw",	tZONE,     HOUR (12) },	/* International Date Line West */
    { "cet",	tZONE,     -HOUR (1) },	/* Central European */
    { "met",	tZONE,     -HOUR (1) },	/* Middle European */
    { "mewt",	tZONE,     -HOUR (1) },	/* Middle European Winter */
    { "mest",	tDAYZONE,  -HOUR (1) },	/* Middle European Summer */
    { "mesz",	tDAYZONE,  -HOUR (1) },	/* Middle European Summer */
    { "swt",	tZONE,     -HOUR (1) },	/* Swedish Winter */
    { "sst",	tDAYZONE,  -HOUR (1) },	/* Swedish Summer */
    { "fwt",	tZONE,     -HOUR (1) },	/* French Winter */
    { "fst",	tDAYZONE,  -HOUR (1) },	/* French Summer */
    { "eet",	tZONE,     -HOUR (2) },	/* Eastern Europe, USSR Zone 1 */
    { "bt",	tZONE,     -HOUR (3) },	/* Baghdad, USSR Zone 2 */
#if 0
    { "it",	tZONE,     -HOUR (3.5) },/* Iran */
#endif
    { "zp4",	tZONE,     -HOUR (4) },	/* USSR Zone 3 */
    { "zp5",	tZONE,     -HOUR (5) },	/* USSR Zone 4 */
#if 0
    { "ist",	tZONE,     -HOUR (5.5) },/* Indian Standard */
#endif
    { "zp6",	tZONE,     -HOUR (6) },	/* USSR Zone 5 */
#if	0
    /* For completeness.  NST is also Newfoundland Standard, and SST is
     * also Swedish Summer. */
    { "nst",	tZONE,     -HOUR (6.5) },/* North Sumatra */
    { "sst",	tZONE,     -HOUR (7) },	/* South Sumatra, USSR Zone 6 */
#endif	/* 0 */
    { "wast",	tZONE,     -HOUR (7) },	/* West Australian Standard */
    { "wadt",	tDAYZONE,  -HOUR (7) },	/* West Australian Daylight */
#if 0
    { "jt",	tZONE,     -HOUR (7.5) },/* Java (3pm in Cronusland!) */
#endif
    { "cct",	tZONE,     -HOUR (8) },	/* China Coast, USSR Zone 7 */
    { "jst",	tZONE,     -HOUR (9) },	/* Japan Standard, USSR Zone 8 */
#if 0
    { "cast",	tZONE,     -HOUR (9.5) },/* Central Australian Standard */
    { "cadt",	tDAYZONE,  -HOUR (9.5) },/* Central Australian Daylight */
#endif
    { "east",	tZONE,     -HOUR (10) },	/* Eastern Australian Standard */
    { "eadt",	tDAYZONE,  -HOUR (10) },	/* Eastern Australian Daylight */
    { "gst",	tZONE,     -HOUR (10) },	/* Guam Standard, USSR Zone 9 */
    { "nzt",	tZONE,     -HOUR (12) },	/* New Zealand */
    { "nzst",	tZONE,     -HOUR (12) },	/* New Zealand Standard */
    { "nzdt",	tDAYZONE,  -HOUR (12) },	/* New Zealand Daylight */
    { "idle",	tZONE,     -HOUR (12) },	/* International Date Line East */
    {  NULL, 0, 0  }
};

/* Military timezone table. */
static TABLE const MilitaryTable[] = {
    { "a",	tZONE,	HOUR (  1) },
    { "b",	tZONE,	HOUR (  2) },
    { "c",	tZONE,	HOUR (  3) },
    { "d",	tZONE,	HOUR (  4) },
    { "e",	tZONE,	HOUR (  5) },
    { "f",	tZONE,	HOUR (  6) },
    { "g",	tZONE,	HOUR (  7) },
    { "h",	tZONE,	HOUR (  8) },
    { "i",	tZONE,	HOUR (  9) },
    { "k",	tZONE,	HOUR ( 10) },
    { "l",	tZONE,	HOUR ( 11) },
    { "m",	tZONE,	HOUR ( 12) },
    { "n",	tZONE,	HOUR (- 1) },
    { "o",	tZONE,	HOUR (- 2) },
    { "p",	tZONE,	HOUR (- 3) },
    { "q",	tZONE,	HOUR (- 4) },
    { "r",	tZONE,	HOUR (- 5) },
    { "s",	tZONE,	HOUR (- 6) },
    { "t",	tZONE,	HOUR (- 7) },
    { "u",	tZONE,	HOUR (- 8) },
    { "v",	tZONE,	HOUR (- 9) },
    { "w",	tZONE,	HOUR (-10) },
    { "x",	tZONE,	HOUR (-11) },
    { "y",	tZONE,	HOUR (-12) },
    { "z",	tZONE,	HOUR (  0) },
    { NULL, 0, 0 }
};




/* ARGSUSED */
static int
yyerror (s)
     char *s ATTRIBUTE_UNUSED;
{
  return 0;
}

static int
ToHour (Hours, Meridian)
     int Hours;
     MERIDIAN Meridian;
{
  switch (Meridian)
    {
    case MER24:
      if (Hours < 0 || Hours > 23)
	return -1;
      return Hours;
    case MERam:
      if (Hours < 1 || Hours > 12)
	return -1;
      if (Hours == 12)
	Hours = 0;
      return Hours;
    case MERpm:
      if (Hours < 1 || Hours > 12)
	return -1;
      if (Hours == 12)
	Hours = 0;
      return Hours + 12;
    default:
      abort ();
    }
  /* NOTREACHED */
}

static int
ToYear (Year)
     int Year;
{
  if (Year < 0)
    Year = -Year;

  /* XPG4 suggests that years 00-68 map to 2000-2068, and
     years 69-99 map to 1969-1999.  */
  if (Year < 69)
    Year += 2000;
  else if (Year < 100)
    Year += 1900;

  return Year;
}

static int
LookupWord (lvalp,buff)
	YYSTYPE *lvalp;
     char *buff;
{
  register char *p;
  register char *q;
  register const TABLE *tp;
  int i;
  int abbrev;

  /* Make it lowercase. */
  for (p = buff; *p; p++)
    if (ISUPPER ((unsigned char) *p))
      *p = tolower (*p);

  if (strcmp (buff, "am") == 0 || strcmp (buff, "a.m.") == 0)
    {
      lvalp->Meridian = MERam;
      return tMERIDIAN;
    }
  if (strcmp (buff, "pm") == 0 || strcmp (buff, "p.m.") == 0)
    {
      lvalp->Meridian = MERpm;
      return tMERIDIAN;
    }

  /* See if we have an abbreviation for a month. */
  if (strlen (buff) == 3)
    abbrev = 1;
  else if (strlen (buff) == 4 && buff[3] == '.')
    {
      abbrev = 1;
      buff[3] = '\0';
    }
  else
    abbrev = 0;

  for (tp = MonthDayTable; tp->name; tp++)
    {
      if (abbrev)
	{
	  if (strncmp (buff, tp->name, 3) == 0)
	    {
	      lvalp->Number = tp->value;
	      return tp->type;
	    }
	}
      else if (strcmp (buff, tp->name) == 0)
	{
	  lvalp->Number = tp->value;
	  return tp->type;
	}
    }

  for (tp = TimezoneTable; tp->name; tp++)
    if (strcmp (buff, tp->name) == 0)
      {
	lvalp->Number = tp->value;
	return tp->type;
      }

  if (strcmp (buff, "dst") == 0)
    return tDST;

  for (tp = UnitsTable; tp->name; tp++)
    if (strcmp (buff, tp->name) == 0)
      {
	lvalp->Number = tp->value;
	return tp->type;
      }

  /* Strip off any plural and try the units table again. */
  i = strlen (buff) - 1;
  if (buff[i] == 's')
    {
      buff[i] = '\0';
      for (tp = UnitsTable; tp->name; tp++)
	if (strcmp (buff, tp->name) == 0)
	  {
	    lvalp->Number = tp->value;
	    return tp->type;
	  }
      buff[i] = 's';		/* Put back for "this" in OtherTable. */
    }

  for (tp = OtherTable; tp->name; tp++)
    if (strcmp (buff, tp->name) == 0)
      {
	lvalp->Number = tp->value;
	return tp->type;
      }

  /* Military timezones. */
  if (buff[1] == '\0' && ISALPHA ((unsigned char) *buff))
    {
      for (tp = MilitaryTable; tp->name; tp++)
	if (strcmp (buff, tp->name) == 0)
	  {
	    lvalp->Number = tp->value;
	    return tp->type;
	  }
    }

  /* Drop out any periods and try the timezone table again. */
  for (i = 0, p = q = buff; *q; q++)
    if (*q != '.')
      *p++ = *q;
    else
      i++;
  *p = '\0';
  if (i)
    for (tp = TimezoneTable; tp->name; tp++)
      if (strcmp (buff, tp->name) == 0)
	{
	  lvalp->Number = tp->value;
	  return tp->type;
	}

  return tID;
}

yylex (YYSTYPE *lvalp, void *parm)
{
  register unsigned char c;
  register char *p;
  char buff[20];
  int Count;
  int sign;
  struct date_yy * date = (struct date_yy *)parm;

  for (;;)
    {
      while (ISSPACE ((unsigned char) *date->yyInput))
	date->yyInput++;

      if (ISDIGIT (c = *date->yyInput) || c == '-' || c == '+')
	{
	  if (c == '-' || c == '+')
	    {
	      sign = c == '-' ? -1 : 1;
	      if (!ISDIGIT (*++date->yyInput))
		/* skip the '-' sign */
		continue;
	    }
	  else
	    sign = 0;
	  for (lvalp->Number = 0; ISDIGIT (c = *date->yyInput++);)
	    lvalp->Number = 10 * lvalp->Number + c - '0';
	  date->yyInput--;
	  if (sign < 0)
	    lvalp->Number = -lvalp->Number;
	  /* Ignore ordinal suffixes on numbers */
	  c = *date->yyInput;
	  if (c == 's' || c == 'n' || c == 'r' || c == 't') {
	    c = *++date->yyInput;
	    if (c == 't' || c == 'd' || c == 'h') {
	      date->yyInput++;
	    } else {
	      date->yyInput--;
	    }
	  }
	  return sign ? tSNUMBER : tUNUMBER;
	}
      if (ISALPHA (c))
	{
	  for (p = buff; (c = *date->yyInput++, ISALPHA (c)) || c == '.';)
	    if (p < &buff[sizeof buff - 1])
	      *p++ = c;
	  *p = '\0';
	  date->yyInput--;
	  return LookupWord (lvalp, buff);
	}
      if (c != '(')
	return *date->yyInput++;
      Count = 0;
      do
	{
	  c = *date->yyInput++;
	  if (c == '\0')
	    return c;
	  if (c == '(')
	    Count++;
	  else if (c == ')')
	    Count--;
	}
      while (Count > 0);
    }
}

#define TM_YEAR_ORIGIN 1900

/* Yield A - B, measured in seconds.  */
static long
difftm (struct tm *a, struct tm *b)
{
  int ay = a->tm_year + (TM_YEAR_ORIGIN - 1);
  int by = b->tm_year + (TM_YEAR_ORIGIN - 1);
  long days = (
  /* difference in day of year */
		a->tm_yday - b->tm_yday
  /* + intervening leap days */
		+ ((ay >> 2) - (by >> 2))
		- (ay / 100 - by / 100)
		+ ((ay / 100 >> 2) - (by / 100 >> 2))
  /* + difference in years * 365 */
		+ (long) (ay - by) * 365
  );
  return (60 * (60 * (24 * days + (a->tm_hour - b->tm_hour))
		+ (a->tm_min - b->tm_min))
	  + (a->tm_sec - b->tm_sec));
}

time_t php_parse_date(char *p, time_t *now)
{
  struct tm tm, tm0, *tmp;
  time_t Start;
  struct date_yy date;

  date.yyInput = p;
  Start = now ? *now : time ((time_t *) NULL);
  tmp = localtime (&Start);
  if (!tmp)
    return -1;
  date.yyYear = tmp->tm_year + TM_YEAR_ORIGIN;
  date.yyMonth = tmp->tm_mon + 1;
  date.yyDay = tmp->tm_mday;
  date.yyHour = tmp->tm_hour;
  date.yyMinutes = tmp->tm_min;
  date.yySeconds = tmp->tm_sec;
  tm.tm_isdst = tmp->tm_isdst;
  date.yyMeridian = MER24;
  date.yyRelSeconds = 0;
  date.yyRelMinutes = 0;
  date.yyRelHour = 0;
  date.yyRelDay = 0;
  date.yyRelMonth = 0;
  date.yyRelYear = 0;
  date.yyHaveDate = 0;
  date.yyHaveDay = 0;
  date.yyHaveRel = 0;
  date.yyHaveTime = 0;
  date.yyHaveZone = 0;

  if (yyparse ((void *)&date)
      || date.yyHaveTime > 1 || date.yyHaveZone > 1 
	  || date.yyHaveDate > 1 || date.yyHaveDay > 1)
    return -1;

  tm.tm_year = ToYear (date.yyYear) - TM_YEAR_ORIGIN + date.yyRelYear;
  tm.tm_mon = date.yyMonth - 1 + date.yyRelMonth;
  tm.tm_mday = date.yyDay + date.yyRelDay;
  if (date.yyHaveTime || (date.yyHaveRel && !date.yyHaveDate && !date.yyHaveDay))
    {
      tm.tm_hour = ToHour (date.yyHour, date.yyMeridian);
      if (tm.tm_hour < 0)
	return -1;
      tm.tm_min = date.yyMinutes;
      tm.tm_sec = date.yySeconds;
    }
  else
    {
      tm.tm_hour = tm.tm_min = tm.tm_sec = 0;
    }
  tm.tm_hour += date.yyRelHour;
  tm.tm_min += date.yyRelMinutes;
  tm.tm_sec += date.yyRelSeconds;

  /* Let mktime deduce tm_isdst if we have an absolute timestamp,
     or if the relative timestamp mentions days, months, or years.  */
  if (date.yyHaveDate | date.yyHaveDay | date.yyHaveTime | date.yyRelDay | date.yyRelMonth | date.yyRelYear)
    tm.tm_isdst = -1;

  tm0 = tm;

  Start = mktime (&tm);

  if (Start == (time_t) -1)
    {

      /* Guard against falsely reporting errors near the time_t boundaries
         when parsing times in other time zones.  For example, if the min
         time_t value is 1970-01-01 00:00:00 UTC and we are 8 hours ahead
         of UTC, then the min localtime value is 1970-01-01 08:00:00; if
         we apply mktime to 1970-01-01 00:00:00 we will get an error, so
         we apply mktime to 1970-01-02 08:00:00 instead and adjust the time
         zone by 24 hours to compensate.  This algorithm assumes that
         there is no DST transition within a day of the time_t boundaries.  */
      if (date.yyHaveZone)
	{
	  tm = tm0;
	  if (tm.tm_year <= EPOCH - TM_YEAR_ORIGIN)
	    {
	      tm.tm_mday++;
	      date.yyTimezone -= 24 * 60;
	    }
	  else
	    {
	      tm.tm_mday--;
	      date.yyTimezone += 24 * 60;
	    }
	  Start = mktime (&tm);
	}

      if (Start == (time_t) -1)
	return Start;
    }

  if (date.yyHaveDay && !date.yyHaveDate)
    {
      tm.tm_mday += ((date.yyDayNumber - tm.tm_wday + 7) % 7
		     + 7 * (date.yyDayOrdinal - (0 < date.yyDayOrdinal)));
      Start = mktime (&tm);
      if (Start == (time_t) -1)
	return Start;
    }

  if (date.yyHaveZone)
    {
      long delta;
      struct tm *gmt = gmtime (&Start);
      if (!gmt)
	return -1;
      delta = date.yyTimezone * 60L + difftm (&tm, gmt);
      if ((Start + delta < Start) != (delta < 0))
	return -1;		/* time_t overflow */
      Start += delta;
    }

  return Start;
}

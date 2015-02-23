/*===========================================================================

This file is temporary and was created to help for testing BC consequences of
potential modifications of ZPP conversion rules

The flags below allow to modify the behavior of the ZPP layer.
In order to customize the ruleset you want to test, just set/unset values
and recompile. No need to run configure again if already done.

As set in PR, the flag values below implement the ruleset proposed in
https://wiki.php.net/rfc/coercive_sth, except the following points, which
are not implemented yet :

	- Changes in the way numeric strings are converted to int/float

============================================================================*/
   
#ifndef ZEND_TMP_STH_H
#define ZEND_TMP_STH_H

/*---- Every flag below can be set/unset in an independent manner, unless
*----- explicitely mentioned */

/* Restrict conversions from IS_NULL */

#define STH_DISABLE_NULL_TO_BOOL	0
#define STH_DISABLE_NULL_TO_INT		0
#define STH_DISABLE_NULL_TO_FLOAT	0
#define STH_DISABLE_NULL_TO_STRING	0

/* Restrict conversions from IS_FALSE/IS_TRUE */

#define STH_DISABLE_BOOL_TO_INT		0
#define STH_DISABLE_BOOL_TO_FLOAT	0
#define STH_DISABLE_BOOL_TO_STRING	0

/* Restrict conversions to bool */

#define STH_DISABLE_INT_TO_BOOL		0
#define STH_DISABLE_FLOAT_TO_BOOL	0
#define STH_DISABLE_STRING_TO_BOOL	0

/* Other restrictions */

#define STH_DISABLE_FLOAT_TO_INT	0

/* Accepts null fractional part only.
   Ignored when STH_DISABLE_FLOAT_TO_INT is set */

#define STH_RESTRICT_FLOAT_TO_INT	0

/*-------------------------------------------------------------------------*/
#endif

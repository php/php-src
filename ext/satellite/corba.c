/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: David Eriksson <david@2good.com>                            |
   +----------------------------------------------------------------------+
 */

/*
 * $Id$
 * vim: syntax=c tabstop=2 shiftwidth=2
 */

/* -----------------------------------------------------------------------
 * 
 * Control access to CORBA_ORB and CORBA_Environment objects
 * Adjust these functions to get thread safety!
 *
 * -----------------------------------------------------------------------
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "corba.h"

#include "php_config.h"	/* for COMPILE_DL_ORBIT */

/* ZTS = thread-safe Zend */
#ifdef ZTS
#error ORBit support not thread-safe, you should at least modify corba.c
#endif

static CORBA_ORB corba_orb = NULL;
static CORBA_Environment corba_environment;
static CORBA_boolean corba_initialized = FALSE;

CORBA_boolean orbit_corba_init()
{
	int argc = 1;
	char * argv[2] = { "dummy", NULL };


#if COMPILE_DL_SATELLITE
	/*
	   IIOP normally registers a function with atexit, but that's pretty stupid
	   when it is called from a dynamic module that is unloaded before exit()
	   is called...
	 
	  	"In addition, for David Eriksson (I think), added:
      				int giop_skip_atexit;
			to IIOP. Applications may set this TRUE prior to invoking any
			CORBA_ORB_init() calls, and it will not invoke atexit. This is
			perhaps an ugly way of doing this, but I'm assuming this feature
			will only be used from within specialized sharedlib applications
			that know what they are doing...

			Kennard"
	 */
	
	giop_skip_atexit = TRUE;
#endif

	CORBA_exception_init(&corba_environment);
	corba_orb = CORBA_ORB_init(
			&argc, argv, "orbit-local-orb", &corba_environment);

	/* check return value */
	if (corba_orb == NULL || orbit_caught_exception())
	{
/*		printf("orbit_corba_init failed\n");*/
		return FALSE;
	}

	corba_initialized = TRUE;

	return TRUE;
}

CORBA_boolean orbit_corba_shutdown()
{
	if (corba_initialized)
	{
		/* the orb's reference count is two.. what to do? */
		CORBA_Object_release((CORBA_Object)corba_orb, &corba_environment);
		CORBA_Object_release((CORBA_Object)corba_orb, &corba_environment);
		/*CORBA_ORB_destroy(corba_orb, &corba_environment); */
		
		CORBA_exception_free(orbit_get_environment());

		corba_initialized = FALSE;
		corba_orb = NULL;
		return TRUE;
	}
	else
		return FALSE;
}

CORBA_ORB orbit_get_orb()
{
	return corba_orb;
}

CORBA_Environment * orbit_get_environment()
{
	return &corba_environment;
}

CORBA_boolean orbit_caught_exception()
{
#if 0 /*debug*/
	if (orbit_get_environment()->_major != CORBA_NO_EXCEPTION)
	{
		printf("Caught exception %s\n", 
				CORBA_exception_id(orbit_get_environment()));
	}
#endif
	return orbit_get_environment()->_major != CORBA_NO_EXCEPTION;
}


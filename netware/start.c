/*=============================================================================
=  CodeWarrior Stationery for Creating a NetWare Executable with NKS/LibC
=
=  Copyright (C) Unpublished Work of Novell, Inc. All Rights Reserved.
=
=  This work is an unpublished work and contains confidential, proprietary and
=  trade  secret information of Novell, Inc. Access to this work is restricted
=  to (i) Novell, Inc. employees who have a need to know how to perform  tasks
=  within  the scope of their assignments and (ii) entities other than Novell,
=  Inc. who have entered into appropriate license agreements. No part of  this
=  work may be used, practiced, performed, copied, distributed, revised, modi-
=  fied, translated, abridged, condensed, expanded, collected, compiled, link-
=  ed,  recast,  transformed  or  adapted without the prior written consent of
=  Novell, Inc. Any use or exploitation of  this  work  without  authorization
=  could subject the perpetrator to criminal and civil liability.
=
=  CodeWarrior is a trademark of Metrowerks Corporation, a Motorola Company.
=  NetWare is a trademark of Novell, Inc.
=
=  When this file is used to instantiate real source code in the CodeWarrior
=  IDE, this copyright notice should be removed and replaced with a different
=  copyright indicating ownership by the developer. Novell assumes no
=  liability for how this sample code is used or modified by any developer.
===============================================================================
*/
#include <library.h>
#include <netware.h>
#include <nks/synch.h>

void			*gLibHandle = (void *) NULL;
rtag_t		gAllocTag = (rtag_t) NULL;
NXMutex_t	*gLibLock = (NXMutex_t *) NULL;
int gLibId=0;

int DisposeLibraryData(	void	*data)
{

	return 0;
}


int _NonAppStart
(
	void			*NLMHandle,
	void			*errorScreen,
	const char	*cmdLine,
	const char	*loadDirPath,
	size_t		uninitializedDataLength,
	void			*NLMFileHandle,
	int			(*readRoutineP)( int conn, void *fileHandle, size_t offset,
								size_t nbytes, size_t *bytesRead, void *buffer ),
	size_t		customDataOffset,
	size_t		customDataSize,
	int			messageCount,
	const char	**messages
)
{
	NX_LOCK_INFO_ALLOC(liblock, "Per-Application Data Lock", 0);

#pragma unused(cmdLine)
#pragma unused(loadDirPath)
#pragma unused(uninitializedDataLength)
#pragma unused(NLMFileHandle)
#pragma unused(readRoutineP)
#pragma unused(customDataOffset)
#pragma unused(customDataSize)
#pragma unused(messageCount)
#pragma unused(messages)

/*
** Here we process our command line, post errors (to the error screen),
** perform initializations and anything else we need to do before being able
** to accept calls into us. If we succeed, we return non-zero and the NetWare
** Loader will leave us up, otherwise we fail to load and get dumped.
*/
//	gAllocTag = AllocateResourceTag(NLMHandle,
//								"<library-name> memory allocations", AllocSignature);

//	if (!gAllocTag)
//	{
//		OutputToScreen(errorScreen, "Unable to allocate resource tag for "
//											"library memory allocations.\n");
//		return -1;
//	}

	gLibId = register_library(DisposeLibraryData);

	if (gLibId < -1)
	{
		OutputToScreen(errorScreen, "Unable to register library with kernel.\n");
		return -1;
	}

	gLibHandle = NLMHandle;

	gLibLock = NXMutexAlloc(0, 0, &liblock);

	if (!gLibLock)
	{
		OutputToScreen(errorScreen, "Unable to allocate library data lock.\n");
		return -1;
	}

	return 0;
}


void _NonAppStop( void )
{
/*
** Here we clean up any resources we allocated. Resource tags is a big part
** of what we created, but NetWare doesn't ask us to free those.
*/
	(void) unregister_library(gLibId);
	NXMutexFree(gLibLock);
}

int  _NonAppCheckUnload( void )
{
/*
** This function cannot be the first in the file for if the file is linked
** first, then the check-unload function's offset will be nlmname.nlm+0
** which is how to tell that there isn't one. When the check function is
** first in the linked objects, it is ambiguous. For this reason, we will
** put it inside this file after the stop function.
**
** Here we check to see if it's alright to ourselves to be unloaded. If not,
** we return a non-zero value. Right now, there isn't any reason not to allow
** it.
*/
	return 0;
}

/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/*
** Handling initialization of the dll library
*/

#include <global.h>
#include <my_sys.h>
#include <my_pthread.h>

static bool libmysql_inited=0;

void libmysql_init(void)
{
  if (libmysql_inited)
    return;
  libmysql_inited=1;
  my_init();
  {
    DBUG_ENTER("libmysql_init");
#ifdef LOG_ALL
    DBUG_PUSH("d:t:S:O,c::\\tmp\\libmysql.log");
#else
    if (getenv("LIBMYSQL_LOG") != NULL)
      DBUG_PUSH(getenv("LIBMYSQL_LOG"));
#endif
    DBUG_VOID_RETURN;
  }
}

#ifdef __WIN__

static int inited=0,threads=0;
HINSTANCE NEAR s_hModule;	/* Saved module handle */
DWORD main_thread;

BOOL APIENTRY LibMain(HANDLE hInst,DWORD ul_reason_being_called,
		      LPVOID lpReserved)
{
  switch (ul_reason_being_called) {
  case DLL_PROCESS_ATTACH:	/* case of libentry call in win 3.x */
    if (!inited++)
    {
      s_hModule=hInst;
      libmysql_init();
      main_thread=GetCurrentThreadId();
    }
    break;
  case DLL_THREAD_ATTACH:
    threads++;
    my_thread_init();
    break;
  case DLL_PROCESS_DETACH:	/* case of wep call in win 3.x */
     if (!--inited)		/* Safety */
     {
       /* my_thread_init() */	/* This may give extra safety */
       my_end(0);
     }
    break;
  case DLL_THREAD_DETACH:
    /* Main thread will free by my_end() */
    threads--;
    if (main_thread != GetCurrentThreadId())
      my_thread_end();
    break;
  default:
    break;
  } /* switch */

  return TRUE;

  UNREFERENCED_PARAMETER(lpReserved);
} /* LibMain */

int __stdcall DllMain(HANDLE hInst,DWORD ul_reason_being_called,LPVOID lpReserved)
{
  return LibMain(hInst,ul_reason_being_called,lpReserved);
}

#elif defined(WINDOWS)

/****************************************************************************
**	This routine is called by LIBSTART.ASM at module load time.  All it
**	does in this sample is remember the DLL module handle.	The module
**	handle is needed if you want to do things like load stuff from the
**	resource file (for instance string resources).
****************************************************************************/

int _export FAR PASCAL libmain(HANDLE hModule,short cbHeapSize,
			       UCHAR FAR *lszCmdLine)
{
  s_hModule = hModule;
  libmysql_init();
  return TRUE;
}

#endif

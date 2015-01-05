/*
** Change here: if you plan to use your own version of <signal.h>
** the original "#include <signal.h>" produces an infinite reinclusion
** of this file, instead of including the standard include-file.
** Under MS Visual Studio, there are occurrences in the source where
** <signal.h> gets included throughout the PHP sources, and this should
** include THIS file, not the standard one which does not have the
** additional signals defined below.
** One way to remove the infinite reinclusion of this file (which is located
** in ../win32), is to specify the parent directory in which the standard
** include file is located.
*/
#include <../include/signal.h>
#define SIGALRM 13
#define	SIGVTALRM 26			/* virtual time alarm */
#define	SIGPROF	27				/* profiling time alarm */

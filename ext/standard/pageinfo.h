#ifndef _PROCESS_H
#define _PROCESS_H

extern void php3_getmyuid(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_getmypid(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_getmyinode(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_getlastmod(INTERNAL_FUNCTION_PARAMETERS);

extern long _php3_getuid(void);

#endif

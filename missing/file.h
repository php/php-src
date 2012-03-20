/* This is file FILE.H */

#ifndef _FILE_H_
#define _FILE_H_

#include <fcntl.h>

#ifndef L_SET
# define L_SET  0       /* seek from beginning.  */
# define L_CURR	1       /* seek from current position.  */
# define L_INCR	1       /* ditto.  */
# define L_XTND 2       /* seek from end.  */
#endif

#ifndef R_OK
# define R_OK  4        /* test whether readable.  */
# define W_OK  2        /* test whether writable.  */
# define X_OK  1        /* test whether execubale. */
# define F_OK  0        /* test whether exist.  */
#endif

#endif

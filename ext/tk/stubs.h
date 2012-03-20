#include <tcl.h>

extern int ruby_open_tcl_dll(char *);
extern int ruby_open_tk_dll();
extern int ruby_open_tcltk_dll(char *);
extern int tcl_stubs_init_p();
extern int tk_stubs_init_p();
extern Tcl_Interp *ruby_tcl_create_ip_and_stubs_init(int*);
extern int ruby_tcl_stubs_init();
extern int ruby_tk_stubs_init(Tcl_Interp*);
extern int ruby_tk_stubs_safeinit(Tcl_Interp*);
extern int ruby_tcltk_stubs();

/* no error */
#define TCLTK_STUBS_OK     (0)

/* return value of ruby_open_tcl_dll() */
#define NO_TCL_DLL         (1)
#define NO_FindExecutable  (2)

/* return value of ruby_open_tk_dll() */
#define NO_TK_DLL         (-1)

/* status value of ruby_tcl_create_ip_and_stubs_init(st) */
#define NO_CreateInterp    (3)
#define NO_DeleteInterp    (4)
#define FAIL_CreateInterp  (5)
#define FAIL_Tcl_InitStubs (6)

/* return value of ruby_tk_stubs_init() */
#define NO_Tk_Init         (7)
#define FAIL_Tk_Init       (8)
#define FAIL_Tk_InitStubs  (9)

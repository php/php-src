#ifndef VIRTUAL_CWD_H
#define VIRTUAL_CWD_H

typedef struct _cwd_state {
	char *cwd;
	int cwd_length;
} cwd_state;

typedef int (*verify_path_func)(const cwd_state *);

void virtual_cwd_init();
char *virtual_getcwd_ex(int *length);
char *virtual_getcwd(char *buf, size_t size);
int virtual_chdir(char *path);
int virtual_filepath(char *path, char **filepath);
FILE *virtual_fopen(char *path, const char *mode);

typedef struct _cwd_globals_struct {
	cwd_state cwd;
} cwd_globals_struct;

#ifdef ZTS
# define CWDLS_D  cwd_globals_struct *cwd_globals
# define CWDLS_DC , CWDLS_D
# define CWDLS_C  cwd_globals
# define CWDLS_CC , CWDLS_C
# define CWDG(v) (cwd_globals->v)
# define CWDLS_FETCH()    cwd_globals_struct *cwd_globals = ts_resource(cwd_globals_id)
CWD_API extern int cwd_globals_id;
#else
# define CWDLS_D  void
# define CWDLS_DC
# define CWDLS_C
# define CWDLS_CC
# define CWDG(v) (cwd_globals.v)
# define CWDLS_FETCH()
extern CWD_API cwd_globals_struct cwd_globals;
#endif

#endif /* VIRTUAL_CWD_H */

#ifndef PWD_H
#define PWD_H

struct passwd {
	char *pw_name;		
	char *pw_passwd;		
	int pw_uid;	
	int pw_gid;	
	char *pw_comment;	
	char *pw_gecos;	
	char *pw_dir;
	char *pw_shell;	
};

extern struct passwd *getpwuid(int);
extern struct passwd *getpwnam(char *name);
extern char *getlogin(void);
#endif

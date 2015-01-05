#define fsync _commit
#define ftruncate chsize

/* For flock() emulation */

#define LOCK_SH 1
#define LOCK_EX 2
#define LOCK_NB 4
#define LOCK_UN 8

PHPAPI int flock(int fd, int op);

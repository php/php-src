
/* pipe related function declarations */

/*#include <unistd.h>*/
#include <proc.h>
#include <fcntl.h>
#include <sys/select.h>


FILE* popen(const char* command, const char* mode);
int pclose(FILE* stream);

/* Borrowed from Apache NT Port */
#include "php.h"

PHPAPI extern char *optarg;
PHPAPI extern int optind;
extern int opterr;
extern int optopt;

PHPAPI int getopt(int argc, char* const *argv, const char *optstr);

/* Borrowed from Apache NT Port */
#include "php.h"

PHPAPI extern char *ap_php_optarg;
PHPAPI extern int ap_php_optind;
extern int ap_php_opterr;
extern int ap_php_optopt;

PHPAPI int ap_php_getopt(int argc, char* const *argv, const char *optstr);

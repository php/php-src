/* Borrowed from Apache NT Port */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "getopt.h"
#define OPTERRCOLON (1)
#define OPTERRNF (2)
#define OPTERRARG (3)


char *ap_optarg;
int ap_optind = 1;
static int ap_opterr = 1;
static int ap_optopt;

static int
ap_optiserr(int argc, char * const *argv, int oint, const char *optstr,
         int optchr, int err)
{
    if (ap_opterr)
    {
        fprintf(stderr, "Error in argument %d, char %d: ", oint, optchr+1);
        switch(err)
        {
        case OPTERRCOLON:
            fprintf(stderr, ": in flags\n");
            break;
        case OPTERRNF:
            fprintf(stderr, "option not found %c\n", argv[oint][optchr]);
            break;
        case OPTERRARG:
            fprintf(stderr, "no argument for option %c\n", argv[oint][optchr]);
            break;
        default:
            fprintf(stderr, "unknown\n");
            break;
        }
    }
    ap_optopt = argv[oint][optchr];
    return('?');
}

int ap_getopt(int argc, char* const *argv, const char *optstr)
{
    static int optchr = 0;
    static int dash = 0; /* have already seen the - */

    char *cp;

    if (ap_optind >= argc)
        return(EOF);
    if (!dash && (argv[ap_optind][0] !=  '-'))
        return(EOF);
    if (!dash && (argv[ap_optind][0] ==  '-') && !argv[ap_optind][1])
    {
        /*
         * use to specify stdin. Need to let pgm process this and
         * the following args
         */
        return(EOF);
    }
    if ((argv[ap_optind][0] == '-') && (argv[ap_optind][1] == '-'))
    {
        /* -- indicates end of args */
        ap_optind++;
        return(EOF);
    }
    if (!dash)
    {
        assert((argv[ap_optind][0] == '-') && argv[ap_optind][1]);
        dash = 1;
        optchr = 1;
    }

    /* Check if the guy tries to do a -: kind of flag */
    assert(dash);
    if (argv[ap_optind][optchr] == ':')
    {
        dash = 0;
        ap_optind++;
        return(ap_optiserr(argc, argv, ap_optind-1, optstr, optchr, OPTERRCOLON));
    }
    if (!(cp = strchr(optstr, argv[ap_optind][optchr])))
    {
        int errind = ap_optind;
        int errchr = optchr;

        if (!argv[ap_optind][optchr+1])
        {
            dash = 0;
            ap_optind++;
        }
        else
            optchr++;
        return(ap_optiserr(argc, argv, errind, optstr, errchr, OPTERRNF));
    }
    if (cp[1] == ':')
    {
        /* Check for cases where the value of the argument
           is in the form -<arg> <val> or in the form -<arg><val> */
        dash = 0;
        if(!argv[ap_optind][2]) {
            ap_optind++;
            if (ap_optind == argc)
                return(ap_optiserr(argc, argv, ap_optind-1, optstr, optchr, OPTERRARG));
            ap_optarg = argv[ap_optind++];
        }
        else
        {
            ap_optarg = &argv[ap_optind][2];
            ap_optind++;
        }
        return(*cp);
    }
    else
    {
        if (!argv[ap_optind][optchr+1])
        {
            dash = 0;
            ap_optind++;
        }
        else
            optchr++;
        return(*cp);
    }
    assert(0);
    return(0);
}

#ifdef TESTGETOPT
int
 main (int argc, char **argv)
 {
      int c;
      extern char *ap_optarg;
      extern int ap_optind;
      int aflg = 0;
      int bflg = 0;
      int errflg = 0;
      char *ofile = NULL;

      while ((c = ap_getopt(argc, argv, "abo:")) != EOF)
           switch (c) {
           case 'a':
                if (bflg)
                     errflg++;
                else
                     aflg++;
                break;
           case 'b':
                if (aflg)
                     errflg++;
                else
                     bflg++;
                break;
           case 'o':
                ofile = ap_optarg;
                (void)printf("ofile = %s\n", ofile);
                break;
           case '?':
                errflg++;
           }
      if (errflg) {
           (void)fprintf(stderr,
                "usage: cmd [-a|-b] [-o <filename>] files...\n");
           exit (2);
      }
      for ( ; ap_optind < argc; ap_optind++)
           (void)printf("%s\n", argv[ap_optind]);
      return 0;
 }

#endif /* TESTGETOPT */

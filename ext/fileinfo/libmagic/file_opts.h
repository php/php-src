/*
 * Table of command-line options
 *
 * The first column specifies the short name, if any, or 0 if none.
 * The second column specifies the long name.
 * The third column specifies whether it takes a parameter.
 * The fourth column is the documentation.
 *
 * N.B. The long options' order must correspond to the code in file.c,
 * and OPTSTRING must be kept up-to-date with the short options.
 * Pay particular attention to the numbers of long-only options in the
 * switch statement!
 */

OPT_LONGONLY("help", 0, "                 display this help and exit\n")
OPT('v', "version", 0, "              output version information and exit\n")
OPT('m', "magic-file", 1, " LIST      use LIST as a colon-separated list of magic\n"
    "                               number files\n")
OPT('z', "uncompress", 0, "           try to look inside compressed files\n")
OPT('b', "brief", 0, "                do not prepend filenames to output lines\n")
OPT('c', "checking-printout", 0, "    print the parsed form of the magic file, use in\n"
    "                               conjunction with -m to debug a new magic file\n"
    "                               before installing it\n")
OPT('e', "exclude", 1, " TEST         exclude TEST from the list of test to be\n"
    "                               performed for file. Valid tests are:\n"
    "                               ascii, apptype, compress, elf, soft, tar, tokens, troff\n")
OPT('f', "files-from", 1, " FILE      read the filenames to be examined from FILE\n")
OPT('F', "separator", 1, " STRING     use string as separator instead of `:'\n")
OPT('i', "mime", 0, "                 output MIME type strings (--mime-type and\n"
    "                               --mime-encoding)\n")
OPT_LONGONLY("mime-type", 0, "            output the MIME type\n")
OPT_LONGONLY("mime-encoding", 0, "        output the MIME encoding\n")
OPT('k', "keep-going", 0, "           don't stop at the first match\n")
#ifdef S_IFLNK
OPT('L', "dereference", 0, "          follow symlinks (default)\n")
OPT('h', "no-dereference", 0, "       don't follow symlinks\n")
#endif
OPT('n', "no-buffer", 0, "            do not buffer output\n")
OPT('N', "no-pad", 0, "               do not pad output\n")
OPT('0', "print0", 0, "               terminate filenames with ASCII NUL\n")
#if defined(HAVE_UTIME) || defined(HAVE_UTIMES)
OPT('p', "preserve-date", 0, "        preserve access times on files\n")
#endif
OPT('r', "raw", 0, "                  don't translate unprintable chars to \\ooo\n")
OPT('s', "special-files", 0, "        treat special (block/char devices) files as\n"
    "                             ordinary ones\n")
OPT('C', "compile", 0, "              compile file specified by -m\n")
OPT('d', "debug", 0, "                print debugging messages\n")

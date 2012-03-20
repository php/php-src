#include <ruby.h>
static void stub_sysinit(int *argc, char ***argv);
#define ruby_sysinit stub_sysinit
#include <main.c>
#undef ruby_sysinit

void
stub_sysinit(int *argc, char ***argv)
{
    char exename[4096];
    size_t lenexe, len0, lenall;
    int i, ac;
    char **av, *p;

    lenexe = (size_t)GetModuleFileName(NULL, exename, sizeof exename);
    ruby_sysinit(argc, argv);
    ac = *argc;
    av = *argv;
    len0 = strlen(av[0]) + 1;
    lenall = 0;
    for (i = 1; i < ac; ++i) {
	lenall += strlen(av[i]) + 1;
    }
    *argv = av = realloc(av, lenall + (lenexe + 1) * 2 + sizeof(char *) * (i + 2));
    *argc = ++ac;
    p = (char *)(av + i + 2);
    memmove(p + (lenexe + 1) * 2, (char *)(av + ac) + len0, lenall);
    memcpy(p, exename, lenexe);
    p[lenexe] = '\0';
    *av++ = p;
    p += lenexe + 1;
    memcpy(p, exename, lenexe);
    p[lenexe] = '\0';
    *av++ = p;
    p += lenexe + 1;
    while (--i) {
	*av++ = p;
	p += strlen(p) + 1;
    }
    *av = NULL;
}


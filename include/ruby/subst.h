#ifndef RUBY_SUBST_H
#define RUBY_SUBST_H 1

#undef snprintf
#undef vsnprintf
#define snprintf ruby_snprintf
#define vsnprintf ruby_vsnprintf

#ifdef BROKEN_CLOSE
#undef getpeername
#define getpeername ruby_getpeername
#undef getsockname
#define getsockname ruby_getsockname
#undef shutdown
#define shutdown ruby_shutdown
#undef close
#define close ruby_close
#endif
#endif

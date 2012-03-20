#if   defined __GNUC__
#warning use "ruby/util.h" instead of bare "util.h"
#elif defined _MSC_VER || defined __BORLANDC__
#pragma message("warning: use \"ruby/util.h\" instead of bare \"util.h\"")
#endif
#include "ruby/util.h"

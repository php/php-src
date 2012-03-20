#if   defined __GNUC__
#warning use "ruby/io.h" instead of "rubyio.h"
#elif defined _MSC_VER || defined __BORLANDC__
#pragma message("warning: use \"ruby/io.h\" instead of \"rubyio.h\"")
#endif
#include "ruby/io.h"

#include "ruby/ruby.h"

void*
dln_load(const char *file)
{
    rb_loaderror("this executable file can't load extension libraries");
}

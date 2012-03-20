#include "ruby.h"
#include "ruby/encoding.h"

VALUE
bug_str_enc_associate(VALUE str, VALUE enc)
{
    return rb_enc_associate(str, rb_to_encoding(enc));
}

void
Init_enc_associate(VALUE klass)
{
    rb_define_method(klass, "associate_encoding!", bug_str_enc_associate, 1);
}

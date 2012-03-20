#include "ruby.h"
#include "ruby/util.h"
#include "ruby/encoding.h"

struct sort_data {
    rb_encoding *enc;
    long elsize;
};

static int
cmp_1(const void *ap, const void *bp, void *dummy)
{
    struct sort_data *d = dummy;
    VALUE a = rb_enc_str_new(ap, d->elsize, d->enc);
    VALUE b = rb_enc_str_new(bp, d->elsize, d->enc);
    VALUE retval = rb_yield_values(2, a, b);
    return rb_cmpint(retval, a, b);
}

static int
cmp_2(const void *ap, const void *bp, void *dummy)
{
    int a = *(const unsigned char *)ap;
    int b = *(const unsigned char *)bp;
    return a - b;
}

static VALUE
bug_str_qsort_bang(int argc, VALUE *argv, VALUE str)
{
    VALUE beg, len, size;
    long l, b = 0, n, s = 1;
    struct sort_data d;

    rb_scan_args(argc, argv, "03", &beg, &len, &size);
    l = RSTRING_LEN(str);
    if (!NIL_P(beg) && (b = NUM2INT(beg)) < 0 && (b += l) < 0) {
	rb_raise(rb_eArgError, "out of bounds");
    }
    if (!NIL_P(size) && (s = NUM2INT(size)) < 0) {
	rb_raise(rb_eArgError, "negative size");
    }
    if (NIL_P(len) ||
	(((n = NUM2INT(len)) < 0) ?
	 (rb_raise(rb_eArgError, "negative length"), 0) :
	 (b + n * s > l))) {
	n = (l - b) / s;
    }
    rb_str_modify(str);
    d.enc = rb_enc_get(str);
    d.elsize = s;
    ruby_qsort(RSTRING_PTR(str) + b, n, s,
	       rb_block_given_p() ? cmp_1 : cmp_2, &d);
    return str;
}

void
Init_qsort(VALUE klass)
{
    rb_define_method(klass, "qsort!", bug_str_qsort_bang, -1);
}

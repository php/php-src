/************************************************

  bubblebabble.c - BubbleBabble encoding support

  $Author$
  created at: Fri Oct 13 18:31:42 JST 2006

  Copyright (C) 2006 Akinori MUSHA

  $Id$

************************************************/

#include "ruby.h"
#include "digest.h"

static ID id_digest;

static VALUE
bubblebabble_str_new(VALUE str_digest)
{
    char *digest;
    size_t digest_len;
    VALUE str;
    char *p;
    size_t i, j, seed = 1;
    static const char vowels[] = {
        'a', 'e', 'i', 'o', 'u', 'y'
    };
    static const char consonants[] = {
        'b', 'c', 'd', 'f', 'g', 'h', 'k', 'l', 'm', 'n',
        'p', 'r', 's', 't', 'v', 'z', 'x'
    };

    StringValue(str_digest);
    digest = RSTRING_PTR(str_digest);
    digest_len = RSTRING_LEN(str_digest);

    if ((LONG_MAX - 2) / 3 < (digest_len | 1)) {
	rb_raise(rb_eRuntimeError, "digest string too long");
    }

    str = rb_str_new(0, (digest_len | 1) * 3 + 2);
    p = RSTRING_PTR(str);

    i = j = 0;
    p[j++] = 'x';

    for (;;) {
        unsigned char byte1, byte2;

        if (i >= digest_len) {
            p[j++] = vowels[seed % 6];
            p[j++] = consonants[16];
            p[j++] = vowels[seed / 6];
            break;
        }

        byte1 = digest[i++];
        p[j++] = vowels[(((byte1 >> 6) & 3) + seed) % 6];
        p[j++] = consonants[(byte1 >> 2) & 15];
        p[j++] = vowels[((byte1 & 3) + (seed / 6)) % 6];

        if (i >= digest_len) {
            break;
        }

        byte2 = digest[i++];
        p[j++] = consonants[(byte2 >> 4) & 15];
        p[j++] = '-';
        p[j++] = consonants[byte2 & 15];

        seed = (seed * 5 + byte1 * 7 + byte2) % 36;
    }

    p[j] = 'x';

    return str;
}

/*
 * call-seq:
 *     Digest.bubblebabble(string) -> bubblebabble_string
 *
 * Returns a BubbleBabble encoded version of a given _string_.
 */
static VALUE
rb_digest_s_bubblebabble(VALUE klass, VALUE str)
{
    return bubblebabble_str_new(str);
}

/*
 * call-seq:
 *     Digest::Class.bubblebabble(string, ...) -> hash_string
 *
 * Returns the BubbleBabble encoded hash value of a given _string_.
 */
static VALUE
rb_digest_class_s_bubblebabble(int argc, VALUE *argv, VALUE klass)
{
    return bubblebabble_str_new(rb_funcall2(klass, id_digest, argc, argv));
}

/*
 * call-seq:
 *     digest_obj.bubblebabble -> hash_string
 *
 * Returns the resulting hash value in a Bubblebabble encoded form.
 */
static VALUE
rb_digest_instance_bubblebabble(VALUE self)
{
    return bubblebabble_str_new(rb_funcall(self, id_digest, 0));
}

/*
 * This module adds some methods to Digest classes to perform
 * BubbleBabble encoding.
 */
void
Init_bubblebabble(void)
{
    VALUE mDigest, mDigest_Instance, cDigest_Class;

    rb_require("digest");

    mDigest = rb_path2class("Digest");
    mDigest_Instance = rb_path2class("Digest::Instance");
    cDigest_Class = rb_path2class("Digest::Class");

    /* Digest::bubblebabble() */
    rb_define_module_function(mDigest, "bubblebabble", rb_digest_s_bubblebabble, 1);

    /* Digest::Class::bubblebabble() */
    rb_define_singleton_method(cDigest_Class, "bubblebabble", rb_digest_class_s_bubblebabble, -1);

    /* Digest::Instance#bubblebabble() */
    rb_define_method(mDigest_Instance, "bubblebabble", rb_digest_instance_bubblebabble, 0);

    id_digest = rb_intern("digest");
}

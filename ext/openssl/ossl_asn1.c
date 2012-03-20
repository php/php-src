/*
 * $Id$
 * 'OpenSSL for Ruby' team members
 * Copyright (C) 2003
 * All rights reserved.
 */
/*
 * This program is licenced under the same licence as Ruby.
 * (See the file 'LICENCE'.)
 */
#include "ossl.h"

#if defined(HAVE_SYS_TIME_H)
#  include <sys/time.h>
#elif !defined(NT) && !defined(_WIN32)
struct timeval {
    long tv_sec;	/* seconds */
    long tv_usec;	/* and microseconds */
};
#endif

static VALUE join_der(VALUE enumerable);
static VALUE ossl_asn1_decode0(unsigned char **pp, long length, long *offset,
			       int depth, int yield, long *num_read);
static VALUE ossl_asn1_initialize(int argc, VALUE *argv, VALUE self);
static VALUE ossl_asn1eoc_initialize(VALUE self);

/*
 * DATE conversion
 */
VALUE
asn1time_to_time(ASN1_TIME *time)
{
    struct tm tm;
    VALUE argv[6];

    if (!time || !time->data) return Qnil;
    memset(&tm, 0, sizeof(struct tm));

    switch (time->type) {
    case V_ASN1_UTCTIME:
	if (sscanf((const char *)time->data, "%2d%2d%2d%2d%2d%2dZ", &tm.tm_year, &tm.tm_mon,
    		&tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 6) {
	    ossl_raise(rb_eTypeError, "bad UTCTIME format");
	}
	if (tm.tm_year < 69) {
	    tm.tm_year += 2000;
	} else {
	    tm.tm_year += 1900;
	}
	break;
    case V_ASN1_GENERALIZEDTIME:
	if (sscanf((const char *)time->data, "%4d%2d%2d%2d%2d%2dZ", &tm.tm_year, &tm.tm_mon,
    		&tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 6) {
	    ossl_raise(rb_eTypeError, "bad GENERALIZEDTIME format" );
	}
	break;
    default:
	rb_warning("unknown time format");
        return Qnil;
    }
    argv[0] = INT2NUM(tm.tm_year);
    argv[1] = INT2NUM(tm.tm_mon);
    argv[2] = INT2NUM(tm.tm_mday);
    argv[3] = INT2NUM(tm.tm_hour);
    argv[4] = INT2NUM(tm.tm_min);
    argv[5] = INT2NUM(tm.tm_sec);

    return rb_funcall2(rb_cTime, rb_intern("utc"), 6, argv);
}

/*
 * This function is not exported in Ruby's *.h
 */
extern struct timeval rb_time_timeval(VALUE);

time_t
time_to_time_t(VALUE time)
{
    return (time_t)NUM2LONG(rb_Integer(time));
}

/*
 * STRING conversion
 */
VALUE
asn1str_to_str(ASN1_STRING *str)
{
    return rb_str_new((const char *)str->data, str->length);
}

/*
 * ASN1_INTEGER conversions
 * TODO: Make a decision what's the right way to do this.
 */
#define DO_IT_VIA_RUBY 0
VALUE
asn1integer_to_num(ASN1_INTEGER *ai)
{
    BIGNUM *bn;
#if DO_IT_VIA_RUBY
    char *txt;
#endif
    VALUE num;

    if (!ai) {
	ossl_raise(rb_eTypeError, "ASN1_INTEGER is NULL!");
    }
    if (!(bn = ASN1_INTEGER_to_BN(ai, NULL))) {
	ossl_raise(eOSSLError, NULL);
    }
#if DO_IT_VIA_RUBY
    if (!(txt = BN_bn2dec(bn))) {
	BN_free(bn);
	ossl_raise(eOSSLError, NULL);
    }
    num = rb_cstr_to_inum(txt, 10, Qtrue);
    OPENSSL_free(txt);
#else
    num = ossl_bn_new(bn);
#endif
    BN_free(bn);

    return num;
}

#if DO_IT_VIA_RUBY
ASN1_INTEGER *
num_to_asn1integer(VALUE obj, ASN1_INTEGER *ai)
{
    BIGNUM *bn = NULL;

    if (RTEST(rb_obj_is_kind_of(obj, cBN))) {
	bn = GetBNPtr(obj);
    } else {
	obj = rb_String(obj);
	if (!BN_dec2bn(&bn, StringValuePtr(obj))) {
	    ossl_raise(eOSSLError, NULL);
	}
    }
    if (!(ai = BN_to_ASN1_INTEGER(bn, ai))) {
	BN_free(bn);
	ossl_raise(eOSSLError, NULL);
    }
    BN_free(bn);
    return ai;
}
#else
ASN1_INTEGER *
num_to_asn1integer(VALUE obj, ASN1_INTEGER *ai)
{
    BIGNUM *bn = GetBNPtr(obj);

    if (!(ai = BN_to_ASN1_INTEGER(bn, ai))) {
	ossl_raise(eOSSLError, NULL);
    }
    return ai;
}
#endif

/********/
/*
 * ASN1 module
 */
#define ossl_asn1_get_value(o)           rb_attr_get((o),sivVALUE)
#define ossl_asn1_get_tag(o)             rb_attr_get((o),sivTAG)
#define ossl_asn1_get_tagging(o)         rb_attr_get((o),sivTAGGING)
#define ossl_asn1_get_tag_class(o)       rb_attr_get((o),sivTAG_CLASS)
#define ossl_asn1_get_infinite_length(o) rb_attr_get((o),sivINFINITE_LENGTH)

#define ossl_asn1_set_value(o,v)           rb_ivar_set((o),sivVALUE,(v))
#define ossl_asn1_set_tag(o,v)             rb_ivar_set((o),sivTAG,(v))
#define ossl_asn1_set_tagging(o,v)         rb_ivar_set((o),sivTAGGING,(v))
#define ossl_asn1_set_tag_class(o,v)       rb_ivar_set((o),sivTAG_CLASS,(v))
#define ossl_asn1_set_infinite_length(o,v) rb_ivar_set((o),sivINFINITE_LENGTH,(v))

VALUE mASN1;
VALUE eASN1Error;

VALUE cASN1Data;
VALUE cASN1Primitive;
VALUE cASN1Constructive;

VALUE cASN1EndOfContent;
VALUE cASN1Boolean;                           /* BOOLEAN           */
VALUE cASN1Integer, cASN1Enumerated;          /* INTEGER           */
VALUE cASN1BitString;                         /* BIT STRING        */
VALUE cASN1OctetString, cASN1UTF8String;      /* STRINGs           */
VALUE cASN1NumericString, cASN1PrintableString;
VALUE cASN1T61String, cASN1VideotexString;
VALUE cASN1IA5String, cASN1GraphicString;
VALUE cASN1ISO64String, cASN1GeneralString;
VALUE cASN1UniversalString, cASN1BMPString;
VALUE cASN1Null;                              /* NULL              */
VALUE cASN1ObjectId;                          /* OBJECT IDENTIFIER */
VALUE cASN1UTCTime, cASN1GeneralizedTime;     /* TIME              */
VALUE cASN1Sequence, cASN1Set;                /* CONSTRUCTIVE      */

static ID sIMPLICIT, sEXPLICIT;
static ID sUNIVERSAL, sAPPLICATION, sCONTEXT_SPECIFIC, sPRIVATE;
static ID sivVALUE, sivTAG, sivTAG_CLASS, sivTAGGING, sivINFINITE_LENGTH, sivUNUSED_BITS;

/*
 * We need to implement these for backward compatibility
 * reasons, behavior of ASN1_put_object and ASN1_object_size
 * for infinite length values is different in OpenSSL <= 0.9.7
 */
#if OPENSSL_VERSION_NUMBER < 0x00908000L
#define ossl_asn1_object_size(cons, len, tag)		(cons) == 2 ? (len) + ASN1_object_size((cons), 0, (tag)) : ASN1_object_size((cons), (len), (tag))
#define ossl_asn1_put_object(pp, cons, len, tag, xc)	(cons) == 2 ? ASN1_put_object((pp), (cons), 0, (tag), (xc)) : ASN1_put_object((pp), (cons), (len), (tag), (xc))
#else
#define ossl_asn1_object_size(cons, len, tag)		ASN1_object_size((cons), (len), (tag))
#define ossl_asn1_put_object(pp, cons, len, tag, xc)	ASN1_put_object((pp), (cons), (len), (tag), (xc))
#endif

/*
 * Ruby to ASN1 converters
 */
static ASN1_BOOLEAN
obj_to_asn1bool(VALUE obj)
{
#if OPENSSL_VERSION_NUMBER < 0x00907000L
     return RTEST(obj) ? 0xff : 0x100;
#else
     return RTEST(obj) ? 0xff : 0x0;
#endif
}

static ASN1_INTEGER*
obj_to_asn1int(VALUE obj)
{
    return num_to_asn1integer(obj, NULL);
}

static ASN1_BIT_STRING*
obj_to_asn1bstr(VALUE obj, long unused_bits)
{
    ASN1_BIT_STRING *bstr;

    if(unused_bits < 0) unused_bits = 0;
    StringValue(obj);
    if(!(bstr = ASN1_BIT_STRING_new()))
	ossl_raise(eASN1Error, NULL);
    ASN1_BIT_STRING_set(bstr, (unsigned char *)RSTRING_PTR(obj), RSTRING_LENINT(obj));
    bstr->flags &= ~(ASN1_STRING_FLAG_BITS_LEFT|0x07); /* clear */
    bstr->flags |= ASN1_STRING_FLAG_BITS_LEFT|(unused_bits&0x07);

    return bstr;
}

static ASN1_STRING*
obj_to_asn1str(VALUE obj)
{
    ASN1_STRING *str;

    StringValue(obj);
    if(!(str = ASN1_STRING_new()))
	ossl_raise(eASN1Error, NULL);
    ASN1_STRING_set(str, RSTRING_PTR(obj), RSTRING_LENINT(obj));

    return str;
}

static ASN1_NULL*
obj_to_asn1null(VALUE obj)
{
    ASN1_NULL *null;

    if(!NIL_P(obj))
	ossl_raise(eASN1Error, "nil expected");
    if(!(null = ASN1_NULL_new()))
	ossl_raise(eASN1Error, NULL);

    return null;
}

static ASN1_OBJECT*
obj_to_asn1obj(VALUE obj)
{
    ASN1_OBJECT *a1obj;

    StringValue(obj);
    a1obj = OBJ_txt2obj(RSTRING_PTR(obj), 0);
    if(!a1obj) a1obj = OBJ_txt2obj(RSTRING_PTR(obj), 1);
    if(!a1obj) ossl_raise(eASN1Error, "invalid OBJECT ID");

    return a1obj;
}

static ASN1_UTCTIME*
obj_to_asn1utime(VALUE time)
{
    time_t sec;
    ASN1_UTCTIME *t;

    sec = time_to_time_t(time);
    if(!(t = ASN1_UTCTIME_set(NULL, sec)))
        ossl_raise(eASN1Error, NULL);

    return t;
}

static ASN1_GENERALIZEDTIME*
obj_to_asn1gtime(VALUE time)
{
    time_t sec;
    ASN1_GENERALIZEDTIME *t;

    sec = time_to_time_t(time);
    if(!(t =ASN1_GENERALIZEDTIME_set(NULL, sec)))
        ossl_raise(eASN1Error, NULL);

    return t;
}

static ASN1_STRING*
obj_to_asn1derstr(VALUE obj)
{
    ASN1_STRING *a1str;
    VALUE str;

    str = ossl_to_der(obj);
    if(!(a1str = ASN1_STRING_new()))
	ossl_raise(eASN1Error, NULL);
    ASN1_STRING_set(a1str, RSTRING_PTR(str), RSTRING_LENINT(str));

    return a1str;
}

/*
 * DER to Ruby converters
 */
static VALUE
decode_bool(unsigned char* der, long length)
{
    int val;
    const unsigned char *p;

    p = der;
    if((val = d2i_ASN1_BOOLEAN(NULL, &p, length)) < 0)
	ossl_raise(eASN1Error, NULL);

    return val ? Qtrue : Qfalse;
}

static VALUE
decode_int(unsigned char* der, long length)
{
    ASN1_INTEGER *ai;
    const unsigned char *p;
    VALUE ret;
    int status = 0;

    p = der;
    if(!(ai = d2i_ASN1_INTEGER(NULL, &p, length)))
	ossl_raise(eASN1Error, NULL);
    ret = rb_protect((VALUE(*)_((VALUE)))asn1integer_to_num,
		     (VALUE)ai, &status);
    ASN1_INTEGER_free(ai);
    if(status) rb_jump_tag(status);

    return ret;
}

static VALUE
decode_bstr(unsigned char* der, long length, long *unused_bits)
{
    ASN1_BIT_STRING *bstr;
    const unsigned char *p;
    long len;
    VALUE ret;

    p = der;
    if(!(bstr = d2i_ASN1_BIT_STRING(NULL, &p, length)))
	ossl_raise(eASN1Error, NULL);
    len = bstr->length;
    *unused_bits = 0;
    if(bstr->flags & ASN1_STRING_FLAG_BITS_LEFT)
	*unused_bits = bstr->flags & 0x07;
    ret = rb_str_new((const char *)bstr->data, len);
    ASN1_BIT_STRING_free(bstr);

    return ret;
}

static VALUE
decode_enum(unsigned char* der, long length)
{
    ASN1_ENUMERATED *ai;
    const unsigned char *p;
    VALUE ret;
    int status = 0;

    p = der;
    if(!(ai = d2i_ASN1_ENUMERATED(NULL, &p, length)))
	ossl_raise(eASN1Error, NULL);
    ret = rb_protect((VALUE(*)_((VALUE)))asn1integer_to_num,
		     (VALUE)ai, &status);
    ASN1_ENUMERATED_free(ai);
    if(status) rb_jump_tag(status);

    return ret;
}

static VALUE
decode_null(unsigned char* der, long length)
{
    ASN1_NULL *null;
    const unsigned char *p;

    p = der;
    if(!(null = d2i_ASN1_NULL(NULL, &p, length)))
	ossl_raise(eASN1Error, NULL);
    ASN1_NULL_free(null);

    return Qnil;
}

static VALUE
decode_obj(unsigned char* der, long length)
{
    ASN1_OBJECT *obj;
    const unsigned char *p;
    VALUE ret;
    int nid;
    BIO *bio;

    p = der;
    if(!(obj = d2i_ASN1_OBJECT(NULL, &p, length)))
	ossl_raise(eASN1Error, NULL);
    if((nid = OBJ_obj2nid(obj)) != NID_undef){
	ASN1_OBJECT_free(obj);
	ret = rb_str_new2(OBJ_nid2sn(nid));
    }
    else{
	if(!(bio = BIO_new(BIO_s_mem()))){
	    ASN1_OBJECT_free(obj);
	    ossl_raise(eASN1Error, NULL);
	}
	i2a_ASN1_OBJECT(bio, obj);
	ASN1_OBJECT_free(obj);
	ret = ossl_membio2str(bio);
    }

    return ret;
}

static VALUE
decode_time(unsigned char* der, long length)
{
    ASN1_TIME *time;
    const unsigned char *p;
    VALUE ret;
    int status = 0;

    p = der;
    if(!(time = d2i_ASN1_TIME(NULL, &p, length)))
	ossl_raise(eASN1Error, NULL);
    ret = rb_protect((VALUE(*)_((VALUE)))asn1time_to_time,
		     (VALUE)time, &status);
    ASN1_TIME_free(time);
    if(status) rb_jump_tag(status);

    return ret;
}

static VALUE
decode_eoc(unsigned char *der, long length)
{
    if (length != 2 || !(der[0] == 0x00 && der[1] == 0x00))
	ossl_raise(eASN1Error, NULL);

    return rb_str_new("", 0);
}

/********/

typedef struct {
    const char *name;
    VALUE *klass;
} ossl_asn1_info_t;

static ossl_asn1_info_t ossl_asn1_info[] = {
    { "EOC",               &cASN1EndOfContent,    },  /*  0 */
    { "BOOLEAN",           &cASN1Boolean,         },  /*  1 */
    { "INTEGER",           &cASN1Integer,         },  /*  2 */
    { "BIT_STRING",        &cASN1BitString,       },  /*  3 */
    { "OCTET_STRING",      &cASN1OctetString,     },  /*  4 */
    { "NULL",              &cASN1Null,            },  /*  5 */
    { "OBJECT",            &cASN1ObjectId,        },  /*  6 */
    { "OBJECT_DESCRIPTOR", NULL,                  },  /*  7 */
    { "EXTERNAL",          NULL,                  },  /*  8 */
    { "REAL",              NULL,                  },  /*  9 */
    { "ENUMERATED",        &cASN1Enumerated,      },  /* 10 */
    { "EMBEDDED_PDV",      NULL,                  },  /* 11 */
    { "UTF8STRING",        &cASN1UTF8String,      },  /* 12 */
    { "RELATIVE_OID",      NULL,                  },  /* 13 */
    { "[UNIVERSAL 14]",    NULL,                  },  /* 14 */
    { "[UNIVERSAL 15]",    NULL,                  },  /* 15 */
    { "SEQUENCE",          &cASN1Sequence,        },  /* 16 */
    { "SET",               &cASN1Set,             },  /* 17 */
    { "NUMERICSTRING",     &cASN1NumericString,   },  /* 18 */
    { "PRINTABLESTRING",   &cASN1PrintableString, },  /* 19 */
    { "T61STRING",         &cASN1T61String,       },  /* 20 */
    { "VIDEOTEXSTRING",    &cASN1VideotexString,  },  /* 21 */
    { "IA5STRING",         &cASN1IA5String,       },  /* 22 */
    { "UTCTIME",           &cASN1UTCTime,         },  /* 23 */
    { "GENERALIZEDTIME",   &cASN1GeneralizedTime, },  /* 24 */
    { "GRAPHICSTRING",     &cASN1GraphicString,   },  /* 25 */
    { "ISO64STRING",       &cASN1ISO64String,     },  /* 26 */
    { "GENERALSTRING",     &cASN1GeneralString,   },  /* 27 */
    { "UNIVERSALSTRING",   &cASN1UniversalString, },  /* 28 */
    { "CHARACTER_STRING",  NULL,                  },  /* 29 */
    { "BMPSTRING",         &cASN1BMPString,       },  /* 30 */
};

int ossl_asn1_info_size = (sizeof(ossl_asn1_info)/sizeof(ossl_asn1_info[0]));

static VALUE class_tag_map;

static int ossl_asn1_default_tag(VALUE obj);

ASN1_TYPE*
ossl_asn1_get_asn1type(VALUE obj)
{
    ASN1_TYPE *ret;
    VALUE value, rflag;
    void *ptr;
    void (*free_func)();
    int tag, flag;

    tag = ossl_asn1_default_tag(obj);
    value = ossl_asn1_get_value(obj);
    switch(tag){
    case V_ASN1_BOOLEAN:
	ptr = (void*)(VALUE)obj_to_asn1bool(value);
	free_func = NULL;
	break;
    case V_ASN1_INTEGER:         /* FALLTHROUGH */
    case V_ASN1_ENUMERATED:
	ptr = obj_to_asn1int(value);
	free_func = ASN1_INTEGER_free;
	break;
    case V_ASN1_BIT_STRING:
        rflag = rb_attr_get(obj, sivUNUSED_BITS);
        flag = NIL_P(rflag) ? -1 : NUM2INT(rflag);
	ptr = obj_to_asn1bstr(value, flag);
	free_func = ASN1_BIT_STRING_free;
	break;
    case V_ASN1_NULL:
	ptr = obj_to_asn1null(value);
	free_func = ASN1_NULL_free;
	break;
    case V_ASN1_OCTET_STRING:    /* FALLTHROUGH */
    case V_ASN1_UTF8STRING:      /* FALLTHROUGH */
    case V_ASN1_NUMERICSTRING:   /* FALLTHROUGH */
    case V_ASN1_PRINTABLESTRING: /* FALLTHROUGH */
    case V_ASN1_T61STRING:       /* FALLTHROUGH */
    case V_ASN1_VIDEOTEXSTRING:  /* FALLTHROUGH */
    case V_ASN1_IA5STRING:       /* FALLTHROUGH */
    case V_ASN1_GRAPHICSTRING:   /* FALLTHROUGH */
    case V_ASN1_ISO64STRING:     /* FALLTHROUGH */
    case V_ASN1_GENERALSTRING:   /* FALLTHROUGH */
    case V_ASN1_UNIVERSALSTRING: /* FALLTHROUGH */
    case V_ASN1_BMPSTRING:
	ptr = obj_to_asn1str(value);
	free_func = ASN1_STRING_free;
	break;
    case V_ASN1_OBJECT:
	ptr = obj_to_asn1obj(value);
	free_func = ASN1_OBJECT_free;
	break;
    case V_ASN1_UTCTIME:
	ptr = obj_to_asn1utime(value);
	free_func = ASN1_TIME_free;
	break;
    case V_ASN1_GENERALIZEDTIME:
	ptr = obj_to_asn1gtime(value);
	free_func = ASN1_TIME_free;
	break;
    case V_ASN1_SET:             /* FALLTHROUGH */
    case V_ASN1_SEQUENCE:
	ptr = obj_to_asn1derstr(obj);
	free_func = ASN1_STRING_free;
	break;
    default:
	ossl_raise(eASN1Error, "unsupported ASN.1 type");
    }
    if(!(ret = OPENSSL_malloc(sizeof(ASN1_TYPE)))){
	if(free_func) free_func(ptr);
	ossl_raise(eASN1Error, "ASN1_TYPE alloc failure");
    }
    memset(ret, 0, sizeof(ASN1_TYPE));
    ASN1_TYPE_set(ret, tag, ptr);

    return ret;
}

static int
ossl_asn1_default_tag(VALUE obj)
{
    VALUE tmp_class, tag;

    tmp_class = CLASS_OF(obj);
    while (tmp_class) {
	tag = rb_hash_lookup(class_tag_map, tmp_class);
	if (tag != Qnil) {
       	    return NUM2INT(tag);
      	}
    	tmp_class = rb_class_superclass(tmp_class);
    }
    ossl_raise(eASN1Error, "universal tag for %s not found",
	       rb_class2name(CLASS_OF(obj)));

    return -1; /* dummy */
}

static int
ossl_asn1_tag(VALUE obj)
{
    VALUE tag;

    tag = ossl_asn1_get_tag(obj);
    if(NIL_P(tag))
	ossl_raise(eASN1Error, "tag number not specified");

    return NUM2INT(tag);
}

static int
ossl_asn1_is_explicit(VALUE obj)
{
    VALUE s;
    int ret = -1;

    s = ossl_asn1_get_tagging(obj);
    if(NIL_P(s)) return 0;
    else if(SYMBOL_P(s)){
	if (SYM2ID(s) == sIMPLICIT)
	    ret = 0;
	else if (SYM2ID(s) == sEXPLICIT)
	    ret = 1;
    }
    if(ret < 0){
	ossl_raise(eASN1Error, "invalid tag default");
    }

    return ret;
}

static int
ossl_asn1_tag_class(VALUE obj)
{
    VALUE s;
    int ret = -1;

    s = ossl_asn1_get_tag_class(obj);
    if(NIL_P(s)) ret = V_ASN1_UNIVERSAL;
    else if(SYMBOL_P(s)){
	if (SYM2ID(s) == sUNIVERSAL)
	    ret = V_ASN1_UNIVERSAL;
	else if (SYM2ID(s) == sAPPLICATION)
	    ret = V_ASN1_APPLICATION;
	else if (SYM2ID(s) == sCONTEXT_SPECIFIC)
	    ret = V_ASN1_CONTEXT_SPECIFIC;
	else if (SYM2ID(s) == sPRIVATE)
	    ret = V_ASN1_PRIVATE;
    }
    if(ret < 0){
	ossl_raise(eASN1Error, "invalid tag class");
    }

    return ret;
}

static VALUE
ossl_asn1_class2sym(int tc)
{
    if((tc & V_ASN1_PRIVATE) == V_ASN1_PRIVATE)
	return ID2SYM(sPRIVATE);
    else if((tc & V_ASN1_CONTEXT_SPECIFIC) == V_ASN1_CONTEXT_SPECIFIC)
	return ID2SYM(sCONTEXT_SPECIFIC);
    else if((tc & V_ASN1_APPLICATION) == V_ASN1_APPLICATION)
	return ID2SYM(sAPPLICATION);
    else
	return ID2SYM(sUNIVERSAL);
}

/*
 * call-seq:
 *    OpenSSL::ASN1::ASN1Data.new(value, tag, tag_class) => ASN1Data
 *
 * +value+: Please have a look at Constructive and Primitive to see how Ruby
 * types are mapped to ASN.1 types and vice versa.
 *
 * +tag+: A +Number+ indicating the tag number.
 *
 * +tag_class+: A +Symbol+ indicating the tag class. Please cf. ASN1 for
 * possible values.
 *
 * == Example
 *   asn1_int = OpenSSL::ASN1Data.new(42, 2, :UNIVERSAL) # => Same as OpenSSL::ASN1::Integer.new(42)
 *   tagged_int = OpenSSL::ASN1Data.new(42, 0, :CONTEXT_SPECIFIC) # implicitly 0-tagged INTEGER
 */
static VALUE
ossl_asn1data_initialize(VALUE self, VALUE value, VALUE tag, VALUE tag_class)
{
    if(!SYMBOL_P(tag_class))
	ossl_raise(eASN1Error, "invalid tag class");
    if((SYM2ID(tag_class) == sUNIVERSAL) && NUM2INT(tag) > 31)
	ossl_raise(eASN1Error, "tag number for Universal too large");
    ossl_asn1_set_tag(self, tag);
    ossl_asn1_set_value(self, value);
    ossl_asn1_set_tag_class(self, tag_class);
    ossl_asn1_set_infinite_length(self, Qfalse);

    return self;
}

static VALUE
join_der_i(VALUE i, VALUE str)
{
    i = ossl_to_der_if_possible(i);
    StringValue(i);
    rb_str_append(str, i);
    return Qnil;
}

static VALUE
join_der(VALUE enumerable)
{
    VALUE str = rb_str_new(0, 0);
    rb_block_call(enumerable, rb_intern("each"), 0, 0, join_der_i, str);
    return str;
}

/*
 * call-seq:
 *    asn1.to_der => DER-encoded String
 *
 * Encodes this ASN1Data into a DER-encoded String value. The result is
 * DER-encoded except for the possibility of infinite length encodings.
 * Infinite length encodings are not allowed in strict DER, so strictly
 * speaking the result of such an encoding would be a BER-encoding.
 */
static VALUE
ossl_asn1data_to_der(VALUE self)
{
    VALUE value, der, inf_length;
    int tag, tag_class, is_cons = 0;
    long length;
    unsigned char *p;

    value = ossl_asn1_get_value(self);
    if(rb_obj_is_kind_of(value, rb_cArray)){
	is_cons = 1;
	value = join_der(value);
    }
    StringValue(value);

    tag = ossl_asn1_tag(self);
    tag_class = ossl_asn1_tag_class(self);
    inf_length = ossl_asn1_get_infinite_length(self);
    if (inf_length == Qtrue) {
	is_cons = 2;
    }
    if((length = ossl_asn1_object_size(is_cons, RSTRING_LENINT(value), tag)) <= 0)
	ossl_raise(eASN1Error, NULL);
    der = rb_str_new(0, length);
    p = (unsigned char *)RSTRING_PTR(der);
    ossl_asn1_put_object(&p, is_cons, RSTRING_LENINT(value), tag, tag_class);
    memcpy(p, RSTRING_PTR(value), RSTRING_LEN(value));
    p += RSTRING_LEN(value);
    ossl_str_adjust(der, p);

    return der;
}

static VALUE
int_ossl_asn1_decode0_prim(unsigned char **pp, long length, long hlen, int tag,
			   VALUE tc, long *num_read)
{
    VALUE value, asn1data;
    unsigned char *p;
    long flag = 0;

    p = *pp;

    if(tc == sUNIVERSAL && tag < ossl_asn1_info_size) {
	switch(tag){
	case V_ASN1_EOC:
	    value = decode_eoc(p, hlen+length);
	    break;
	case V_ASN1_BOOLEAN:
	    value = decode_bool(p, hlen+length);
	    break;
	case V_ASN1_INTEGER:
	    value = decode_int(p, hlen+length);
	    break;
	case V_ASN1_BIT_STRING:
	    value = decode_bstr(p, hlen+length, &flag);
	    break;
	case V_ASN1_NULL:
	    value = decode_null(p, hlen+length);
	    break;
	case V_ASN1_ENUMERATED:
	    value = decode_enum(p, hlen+length);
	    break;
	case V_ASN1_OBJECT:
	    value = decode_obj(p, hlen+length);
	    break;
	case V_ASN1_UTCTIME:           /* FALLTHROUGH */
	case V_ASN1_GENERALIZEDTIME:
	    value = decode_time(p, hlen+length);
	    break;
	default:
	    /* use original value */
	    p += hlen;
	    value = rb_str_new((const char *)p, length);
	    break;
	}
    }
    else {
	p += hlen;
	value = rb_str_new((const char *)p, length);
    }

    *pp += hlen + length;
    *num_read = hlen + length;

    if (tc == sUNIVERSAL && tag < ossl_asn1_info_size && ossl_asn1_info[tag].klass) {
	VALUE klass = *ossl_asn1_info[tag].klass;
	VALUE args[4];
	args[0] = value;
	args[1] = INT2NUM(tag);
	args[2] = Qnil;
	args[3] = ID2SYM(tc);
	asn1data = rb_obj_alloc(klass);
	ossl_asn1_initialize(4, args, asn1data);
	if(tag == V_ASN1_BIT_STRING){
	    rb_ivar_set(asn1data, sivUNUSED_BITS, LONG2NUM(flag));
	}
    }
    else {
	asn1data = rb_obj_alloc(cASN1Data);
	ossl_asn1data_initialize(asn1data, value, INT2NUM(tag), ID2SYM(tc));
    }

    return asn1data;
}

static VALUE
int_ossl_asn1_decode0_cons(unsigned char **pp, long max_len, long length,
			   long *offset, int depth, int yield, int j,
			   int tag, VALUE tc, long *num_read)
{
    VALUE value, asn1data, ary;
    int infinite;
    long off = *offset;

    infinite = (j == 0x21);
    ary = rb_ary_new();

    while (length > 0 || infinite) {
	long inner_read = 0;
	value = ossl_asn1_decode0(pp, max_len, &off, depth + 1, yield, &inner_read);
	*num_read += inner_read;
	max_len -= inner_read;
	rb_ary_push(ary, value);
	if (length > 0)
	    length -= inner_read;

	if (infinite &&
	    NUM2INT(ossl_asn1_get_tag(value)) == V_ASN1_EOC &&
	    SYM2ID(ossl_asn1_get_tag_class(value)) == sUNIVERSAL) {
	    break;
	}
    }

    if (tc == sUNIVERSAL) {
	VALUE args[4];
	int not_sequence_or_set;

	not_sequence_or_set = tag != V_ASN1_SEQUENCE && tag != V_ASN1_SET;

	if (not_sequence_or_set) {
	    if (infinite) {
		asn1data = rb_obj_alloc(cASN1Constructive);
	    }
	    else {
		ossl_raise(eASN1Error, "invalid non-infinite tag");
		return Qnil;
	    }
	}
	else {
	    VALUE klass = *ossl_asn1_info[tag].klass;
	    asn1data = rb_obj_alloc(klass);
	}
	args[0] = ary;
	args[1] = INT2NUM(tag);
	args[2] = Qnil;
	args[3] = ID2SYM(tc);
	ossl_asn1_initialize(4, args, asn1data);
    }
    else {
	asn1data = rb_obj_alloc(cASN1Data);
	ossl_asn1data_initialize(asn1data, ary, INT2NUM(tag), ID2SYM(tc));
    }

    if (infinite)
	ossl_asn1_set_infinite_length(asn1data, Qtrue);
    else
	ossl_asn1_set_infinite_length(asn1data, Qfalse);

    *offset = off;
    return asn1data;
}

static VALUE
ossl_asn1_decode0(unsigned char **pp, long length, long *offset, int depth,
		  int yield, long *num_read)
{
    unsigned char *start, *p;
    const unsigned char *p0;
    long len = 0, inner_read = 0, off = *offset, hlen;
    int tag, tc, j;
    VALUE asn1data, tag_class;

    p = *pp;
    start = p;
    p0 = p;
    j = ASN1_get_object(&p0, &len, &tag, &tc, length);
    p = (unsigned char *)p0;
    if(j & 0x80) ossl_raise(eASN1Error, NULL);
    if(len > length) ossl_raise(eASN1Error, "value is too short");
    if((tc & V_ASN1_PRIVATE) == V_ASN1_PRIVATE)
	tag_class = sPRIVATE;
    else if((tc & V_ASN1_CONTEXT_SPECIFIC) == V_ASN1_CONTEXT_SPECIFIC)
	tag_class = sCONTEXT_SPECIFIC;
    else if((tc & V_ASN1_APPLICATION) == V_ASN1_APPLICATION)
	tag_class = sAPPLICATION;
    else
	tag_class = sUNIVERSAL;

    hlen = p - start;

    if(yield) {
	VALUE arg = rb_ary_new();
	rb_ary_push(arg, LONG2NUM(depth));
	rb_ary_push(arg, LONG2NUM(*offset));
	rb_ary_push(arg, LONG2NUM(hlen));
	rb_ary_push(arg, LONG2NUM(len));
	rb_ary_push(arg, (j & V_ASN1_CONSTRUCTED) ? Qtrue : Qfalse);
	rb_ary_push(arg, ossl_asn1_class2sym(tc));
	rb_ary_push(arg, INT2NUM(tag));
	rb_yield(arg);
    }

    if(j & V_ASN1_CONSTRUCTED) {
	*pp += hlen;
	off += hlen;
	asn1data = int_ossl_asn1_decode0_cons(pp, length, len, &off, depth, yield, j, tag, tag_class, &inner_read);
	inner_read += hlen;
    }
    else {
    	if ((j & 0x01) && (len == 0)) ossl_raise(eASN1Error, "Infinite length for primitive value");
	asn1data = int_ossl_asn1_decode0_prim(pp, len, hlen, tag, tag_class, &inner_read);
	off += hlen + len;
    }
    if (num_read)
	*num_read = inner_read;
    if (len != 0 && inner_read != hlen + len) {
	ossl_raise(eASN1Error,
		   "Type mismatch. Bytes read: %ld Bytes available: %ld",
		   inner_read, hlen + len);
    }

    *offset = off;
    return asn1data;
}

static void
int_ossl_decode_sanity_check(long len, long read, long offset)
{
    if (len != 0 && (read != len || offset != len)) {
	ossl_raise(eASN1Error,
		   "Type mismatch. Total bytes read: %ld Bytes available: %ld Offset: %ld",
		   read, len, offset);
    }
}

/*
 * call-seq:
 *    OpenSSL::ASN1.traverse(asn1) -> nil
 *
 * If a block is given, it prints out each of the elements encountered.
 * Block parameters are (in that order):
 * * depth: The recursion depth, plus one with each constructed value being encountered (Number)
 * * offset: Current byte offset (Number)
 * * header length: Combined length in bytes of the Tag and Length headers. (Number)
 * * length: The overall remaining length of the entire data (Number)
 * * constructed: Whether this value is constructed or not (Boolean)
 * * tag_class: Current tag class (Symbol)
 * * tag: The current tag (Number)
 *
 * == Example
 *   der = File.binread('asn1data.der')
 *   OpenSSL::ASN1.traverse(der) do | depth, offset, header_len, length, constructed, tag_class, tag|
 *     puts "Depth: #{depth} Offset: #{offset} Length: #{length}"
 *     puts "Header length: #{header_len} Tag: #{tag} Tag class: #{tag_class} Constructed: #{constructed}"
 *   end
 */
static VALUE
ossl_asn1_traverse(VALUE self, VALUE obj)
{
    unsigned char *p;
    volatile VALUE tmp;
    long len, read = 0, offset = 0;

    obj = ossl_to_der_if_possible(obj);
    tmp = rb_str_new4(StringValue(obj));
    p = (unsigned char *)RSTRING_PTR(tmp);
    len = RSTRING_LEN(tmp);
    ossl_asn1_decode0(&p, len, &offset, 0, 1, &read);
    int_ossl_decode_sanity_check(len, read, offset);
    return Qnil;
}

/*
 * call-seq:
 *    OpenSSL::ASN1.decode(der) -> ASN1Data
 *
 * Decodes a BER- or DER-encoded value and creates an ASN1Data instance. +der+
 * may be a +String+ or any object that features a +#to_der+ method transforming
 * it into a BER-/DER-encoded +String+.
 *
 * == Example
 *   der = File.binread('asn1data')
 *   asn1 = OpenSSL::ASN1.decode(der)
 */
static VALUE
ossl_asn1_decode(VALUE self, VALUE obj)
{
    VALUE ret;
    unsigned char *p;
    volatile VALUE tmp;
    long len, read = 0, offset = 0;

    obj = ossl_to_der_if_possible(obj);
    tmp = rb_str_new4(StringValue(obj));
    p = (unsigned char *)RSTRING_PTR(tmp);
    len = RSTRING_LEN(tmp);
    ret = ossl_asn1_decode0(&p, len, &offset, 0, 0, &read);
    int_ossl_decode_sanity_check(len, read, offset);
    return ret;
}

/*
 * call-seq:
 *    OpenSSL::ASN1.decode_all(der) -> Array of ASN1Data
 *
 * Similar to +decode+ with the difference that +decode+ expects one
 * distinct value represented in +der+. +decode_all+ on the contrary
 * decodes a sequence of sequential BER/DER values lined up in +der+
 * and returns them as an array.
 *
 * == Example
 *   ders = File.binread('asn1data_seq')
 *   asn1_ary = OpenSSL::ASN1.decode_all(ders)
 */
static VALUE
ossl_asn1_decode_all(VALUE self, VALUE obj)
{
    VALUE ary, val;
    unsigned char *p;
    long len, tmp_len = 0, read = 0, offset = 0;
    volatile VALUE tmp;

    obj = ossl_to_der_if_possible(obj);
    tmp = rb_str_new4(StringValue(obj));
    p = (unsigned char *)RSTRING_PTR(tmp);
    len = RSTRING_LEN(tmp);
    tmp_len = len;
    ary = rb_ary_new();
    while (tmp_len > 0) {
	long tmp_read = 0;
	val = ossl_asn1_decode0(&p, tmp_len, &offset, 0, 0, &tmp_read);
	rb_ary_push(ary, val);
	read += tmp_read;
	tmp_len -= tmp_read;
    }
    int_ossl_decode_sanity_check(len, read, offset);
    return ary;
}

/*
 * call-seq:
 *    OpenSSL::ASN1::Primitive.new( value [, tag, tagging, tag_class ]) => Primitive
 *
 * +value+: is mandatory.
 *
 * +tag+: optional, may be specified for tagged values. If no +tag+ is
 * specified, the UNIVERSAL tag corresponding to the Primitive sub-class
 * is used by default.
 *
 * +tagging+: may be used as an encoding hint to encode a value either
 * explicitly or implicitly, see ASN1 for possible values.
 *
 * +tag_class+: if +tag+ and +tagging+ are +nil+ then this is set to
 * +:UNIVERSAL+ by default. If either +tag+ or +tagging+ are set then
 * +:CONTEXT_SPECIFIC+ is used as the default. For possible values please
 * cf. ASN1.
 *
 * == Example
 *   int = OpenSSL::ASN1::Integer.new(42)
 *   zero_tagged_int = OpenSSL::ASN1::Integer.new(42, 0, :IMPLICIT)
 *   private_explicit_zero_tagged_int = OpenSSL::ASN1::Integer.new(42, 0, :EXPLICIT, :PRIVATE)
 */
static VALUE
ossl_asn1_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE value, tag, tagging, tag_class;

    rb_scan_args(argc, argv, "13", &value, &tag, &tagging, &tag_class);
    if(argc > 1){
	if(NIL_P(tag))
	    ossl_raise(eASN1Error, "must specify tag number");
	if(!NIL_P(tagging) && !SYMBOL_P(tagging))
	    ossl_raise(eASN1Error, "invalid tagging method");
	if(NIL_P(tag_class)) {
	    if (NIL_P(tagging))
		tag_class = ID2SYM(sUNIVERSAL);
	    else
		tag_class = ID2SYM(sCONTEXT_SPECIFIC);
	}
	if(!SYMBOL_P(tag_class))
	    ossl_raise(eASN1Error, "invalid tag class");
	if(SYM2ID(tagging) == sIMPLICIT && NUM2INT(tag) > 31)
	    ossl_raise(eASN1Error, "tag number for Universal too large");
    }
    else{
	tag = INT2NUM(ossl_asn1_default_tag(self));
	tagging = Qnil;
	tag_class = ID2SYM(sUNIVERSAL);
    }
    ossl_asn1_set_tag(self, tag);
    ossl_asn1_set_value(self, value);
    ossl_asn1_set_tagging(self, tagging);
    ossl_asn1_set_tag_class(self, tag_class);
    ossl_asn1_set_infinite_length(self, Qfalse);

    return self;
}

static VALUE
ossl_asn1eoc_initialize(VALUE self) {
    VALUE tag, tagging, tag_class, value;
    tag = INT2NUM(ossl_asn1_default_tag(self));
    tagging = Qnil;
    tag_class = ID2SYM(sUNIVERSAL);
    value = rb_str_new("", 0);
    ossl_asn1_set_tag(self, tag);
    ossl_asn1_set_value(self, value);
    ossl_asn1_set_tagging(self, tagging);
    ossl_asn1_set_tag_class(self, tag_class);
    ossl_asn1_set_infinite_length(self, Qfalse);
    return self;
}

static int
ossl_i2d_ASN1_TYPE(ASN1_TYPE *a, unsigned char **pp)
{
#if OPENSSL_VERSION_NUMBER < 0x00907000L
    if(!a) return 0;
    if(a->type == V_ASN1_BOOLEAN)
        return i2d_ASN1_BOOLEAN(a->value.boolean, pp);
#endif
    return i2d_ASN1_TYPE(a, pp);
}

static void
ossl_ASN1_TYPE_free(ASN1_TYPE *a)
{
#if OPENSSL_VERSION_NUMBER < 0x00907000L
    if(!a) return;
    if(a->type == V_ASN1_BOOLEAN){
        OPENSSL_free(a);
        return;
    }
#endif
    ASN1_TYPE_free(a);
}

/*
 * call-seq:
 *    asn1.to_der => DER-encoded String
 *
 * See ASN1Data#to_der for details. *
 */
static VALUE
ossl_asn1prim_to_der(VALUE self)
{
    ASN1_TYPE *asn1;
    int tn, tc, explicit;
    long len, reallen;
    unsigned char *buf, *p;
    VALUE str;

    tn = NUM2INT(ossl_asn1_get_tag(self));
    tc = ossl_asn1_tag_class(self);
    explicit = ossl_asn1_is_explicit(self);
    asn1 = ossl_asn1_get_asn1type(self);

    len = ossl_asn1_object_size(1, ossl_i2d_ASN1_TYPE(asn1, NULL), tn);
    if(!(buf = OPENSSL_malloc(len))){
	ossl_ASN1_TYPE_free(asn1);
	ossl_raise(eASN1Error, "cannot alloc buffer");
    }
    p = buf;
    if (tc == V_ASN1_UNIVERSAL) {
        ossl_i2d_ASN1_TYPE(asn1, &p);
    } else if (explicit) {
        ossl_asn1_put_object(&p, 1, ossl_i2d_ASN1_TYPE(asn1, NULL), tn, tc);
        ossl_i2d_ASN1_TYPE(asn1, &p);
    } else {
        ossl_i2d_ASN1_TYPE(asn1, &p);
        *buf = tc | tn | (*buf & V_ASN1_CONSTRUCTED);
    }
    ossl_ASN1_TYPE_free(asn1);
    reallen = p - buf;
    assert(reallen <= len);
    str = ossl_buf2str((char *)buf, rb_long2int(reallen)); /* buf will be free in ossl_buf2str */

    return str;
}

/*
 * call-seq:
 *    asn1.to_der => DER-encoded String
 *
 * See ASN1Data#to_der for details.
 */
static VALUE
ossl_asn1cons_to_der(VALUE self)
{
    int tag, tn, tc, explicit, constructed = 1;
    int found_prim = 0, seq_len;
    long length;
    unsigned char *p;
    VALUE value, str, inf_length;

    tn = NUM2INT(ossl_asn1_get_tag(self));
    tc = ossl_asn1_tag_class(self);
    inf_length = ossl_asn1_get_infinite_length(self);
    if (inf_length == Qtrue) {
	VALUE ary, example;
	constructed = 2;
	if (CLASS_OF(self) == cASN1Sequence ||
	    CLASS_OF(self) == cASN1Set) {
	    tag = ossl_asn1_default_tag(self);
	}
	else { /* must be a constructive encoding of a primitive value */
	    ary = ossl_asn1_get_value(self);
	    if (!rb_obj_is_kind_of(ary, rb_cArray))
		ossl_raise(eASN1Error, "Constructive value must be an Array");
	    /* Recursively descend until a primitive value is found.
	    The overall value of the entire constructed encoding
	    is of the type of the first primitive encoding to be
	    found. */
	    while (!found_prim){
		example = rb_ary_entry(ary, 0);
		if (rb_obj_is_kind_of(example, cASN1Primitive)){
		    found_prim = 1;
		}
		else {
		    /* example is another ASN1Constructive */
		    if (!rb_obj_is_kind_of(example, cASN1Constructive)){
			ossl_raise(eASN1Error, "invalid constructed encoding");
			return Qnil; /* dummy */
		    }
		    ary = ossl_asn1_get_value(example);
		}
	    }
	    tag = ossl_asn1_default_tag(example);
	}
    }
    else {
	if (CLASS_OF(self) == cASN1Constructive)
	    ossl_raise(eASN1Error, "Constructive shall only be used with infinite length");
	tag = ossl_asn1_default_tag(self);
    }
    explicit = ossl_asn1_is_explicit(self);
    value = join_der(ossl_asn1_get_value(self));

    seq_len = ossl_asn1_object_size(constructed, RSTRING_LENINT(value), tag);
    length = ossl_asn1_object_size(constructed, seq_len, tn);
    str = rb_str_new(0, length);
    p = (unsigned char *)RSTRING_PTR(str);
    if(tc == V_ASN1_UNIVERSAL)
	ossl_asn1_put_object(&p, constructed, RSTRING_LENINT(value), tn, tc);
    else{
	if(explicit){
	    ossl_asn1_put_object(&p, constructed, seq_len, tn, tc);
	    ossl_asn1_put_object(&p, constructed, RSTRING_LENINT(value), tag, V_ASN1_UNIVERSAL);
	}
	else{
	    ossl_asn1_put_object(&p, constructed, RSTRING_LENINT(value), tn, tc);
	}
    }
    memcpy(p, RSTRING_PTR(value), RSTRING_LEN(value));
    p += RSTRING_LEN(value);

    /* In this case we need an additional EOC (one for the explicit part and
     * one for the Constructive itself. The EOC for the Constructive is
     * supplied by the user, but that for the "explicit wrapper" must be
     * added here.
     */
    if (explicit && inf_length == Qtrue) {
	ASN1_put_eoc(&p);
    }
    ossl_str_adjust(str, p);

    return str;
}

/*
 * call-seq:
 *    asn1_ary.each { |asn1| block } => asn1_ary
 *
 * Calls <i>block</i> once for each element in +self+, passing that element
 * as parameter +asn1+. If no block is given, an enumerator is returned
 * instead.
 *
 * == Example
 *   asn1_ary.each do |asn1|
 *     puts asn1
 *   end
 */
static VALUE
ossl_asn1cons_each(VALUE self)
{
    rb_ary_each(ossl_asn1_get_value(self));
    return self;
}

static VALUE
ossl_asn1obj_s_register(VALUE self, VALUE oid, VALUE sn, VALUE ln)
{
    StringValue(oid);
    StringValue(sn);
    StringValue(ln);

    if(!OBJ_create(RSTRING_PTR(oid), RSTRING_PTR(sn), RSTRING_PTR(ln)))
	ossl_raise(eASN1Error, NULL);

    return Qtrue;
}

static VALUE
ossl_asn1obj_get_sn(VALUE self)
{
    VALUE val, ret = Qnil;
    int nid;

    val = ossl_asn1_get_value(self);
    if ((nid = OBJ_txt2nid(StringValuePtr(val))) != NID_undef)
	ret = rb_str_new2(OBJ_nid2sn(nid));

    return ret;
}

static VALUE
ossl_asn1obj_get_ln(VALUE self)
{
    VALUE val, ret = Qnil;
    int nid;

    val = ossl_asn1_get_value(self);
    if ((nid = OBJ_txt2nid(StringValuePtr(val))) != NID_undef)
	ret = rb_str_new2(OBJ_nid2ln(nid));

    return ret;
}

static VALUE
ossl_asn1obj_get_oid(VALUE self)
{
    VALUE val;
    ASN1_OBJECT *a1obj;
    char buf[128];

    val = ossl_asn1_get_value(self);
    a1obj = obj_to_asn1obj(val);
    OBJ_obj2txt(buf, sizeof(buf), a1obj, 1);
    ASN1_OBJECT_free(a1obj);

    return rb_str_new2(buf);
}

#define OSSL_ASN1_IMPL_FACTORY_METHOD(klass) \
static VALUE ossl_asn1_##klass(int argc, VALUE *argv, VALUE self)\
{ return rb_funcall3(cASN1##klass, rb_intern("new"), argc, argv); }

OSSL_ASN1_IMPL_FACTORY_METHOD(Boolean)
OSSL_ASN1_IMPL_FACTORY_METHOD(Integer)
OSSL_ASN1_IMPL_FACTORY_METHOD(Enumerated)
OSSL_ASN1_IMPL_FACTORY_METHOD(BitString)
OSSL_ASN1_IMPL_FACTORY_METHOD(OctetString)
OSSL_ASN1_IMPL_FACTORY_METHOD(UTF8String)
OSSL_ASN1_IMPL_FACTORY_METHOD(NumericString)
OSSL_ASN1_IMPL_FACTORY_METHOD(PrintableString)
OSSL_ASN1_IMPL_FACTORY_METHOD(T61String)
OSSL_ASN1_IMPL_FACTORY_METHOD(VideotexString)
OSSL_ASN1_IMPL_FACTORY_METHOD(IA5String)
OSSL_ASN1_IMPL_FACTORY_METHOD(GraphicString)
OSSL_ASN1_IMPL_FACTORY_METHOD(ISO64String)
OSSL_ASN1_IMPL_FACTORY_METHOD(GeneralString)
OSSL_ASN1_IMPL_FACTORY_METHOD(UniversalString)
OSSL_ASN1_IMPL_FACTORY_METHOD(BMPString)
OSSL_ASN1_IMPL_FACTORY_METHOD(Null)
OSSL_ASN1_IMPL_FACTORY_METHOD(ObjectId)
OSSL_ASN1_IMPL_FACTORY_METHOD(UTCTime)
OSSL_ASN1_IMPL_FACTORY_METHOD(GeneralizedTime)
OSSL_ASN1_IMPL_FACTORY_METHOD(Sequence)
OSSL_ASN1_IMPL_FACTORY_METHOD(Set)
OSSL_ASN1_IMPL_FACTORY_METHOD(EndOfContent)

void
Init_ossl_asn1()
{
    VALUE ary;
    int i;

#if 0
    mOSSL = rb_define_module("OpenSSL"); /* let rdoc know about mOSSL */
#endif

    sUNIVERSAL = rb_intern("UNIVERSAL");
    sCONTEXT_SPECIFIC = rb_intern("CONTEXT_SPECIFIC");
    sAPPLICATION = rb_intern("APPLICATION");
    sPRIVATE = rb_intern("PRIVATE");
    sEXPLICIT = rb_intern("EXPLICIT");
    sIMPLICIT = rb_intern("IMPLICIT");

    sivVALUE = rb_intern("@value");
    sivTAG = rb_intern("@tag");
    sivTAGGING = rb_intern("@tagging");
    sivTAG_CLASS = rb_intern("@tag_class");
    sivINFINITE_LENGTH = rb_intern("@infinite_length");
    sivUNUSED_BITS = rb_intern("@unused_bits");

    /*
     * Document-module: OpenSSL::ASN1
     *
     * Abstract Syntax Notation One (or ASN.1) is a notation syntax to
     * describe data structures and is defined in ITU-T X.680. ASN.1 itself
     * does not mandate any encoding or parsing rules, but usually ASN.1 data
     * structures are encoded using the Distinguished Encoding Rules (DER) or
     * less often the Basic Encoding Rules (BER) described in ITU-T X.690. DER
     * and BER encodings are binary Tag-Length-Value (TLV) encodings that are
     * quite concise compared to other popular data description formats such
     * as XML, JSON etc.
     * ASN.1 data structures are very common in cryptographic applications,
     * e.g. X.509 public key certificates or certificate revocation lists
     * (CRLs) are all defined in ASN.1 and DER-encoded. ASN.1, DER and BER are
     * the building blocks of applied cryptography.
     * The ASN1 module provides the necessary classes that allow generation
     * of ASN.1 data structures and the methods to encode them using a DER
     * encoding. The decode method allows parsing arbitrary BER-/DER-encoded
     * data to a Ruby object that can then be modified and re-encoded at will.
     *
     * == ASN.1 class hierarchy
     *
     * The base class representing ASN.1 structures is ASN1Data. ASN1Data offers
     * attributes to read and set the +tag+, the +tag_class+ and finally the
     * +value+ of a particular ASN.1 item. Upon parsing, any tagged values
     * (implicit or explicit) will be represented by ASN1Data instances because
     * their "real type" can only be determined using out-of-band information
     * from the ASN.1 type declaration. Since this information is normally
     * known when encoding a type, all sub-classes of ASN1Data offer an
     * additional attribute +tagging+ that allows to encode a value implicitly
     * (+:IMPLICIT+) or explicitly (+:EXPLICIT+).
     *
     * === Constructive
     *
     * Constructive is, as its name implies, the base class for all
     * constructed encodings, i.e. those that consist of several values,
     * opposed to "primitive" encodings with just one single value.
     * Primitive values that are encoded with "infinite length" are typically
     * constructed (their values come in multiple chunks) and are therefore
     * represented by instances of Constructive. The value of an Constructive
     * is always an Array.
     *
     * ==== ASN1::Set and ASN1::Sequence
     *
     * The most common constructive encodings are SETs and SEQUENCEs, which is
     * why there are two sub-classes of Constructive representing each of
     * them.
     *
     * === Primitive
     *
     * This is the super class of all primitive values. Primitive
     * itself is not used when parsing ASN.1 data, all values are either
     * instances of a corresponding sub-class of Primitive or they are
     * instances of ASN1Data if the value was tagged implicitly or explicitly.
     * Please cf. Primitive documentation for details on sub-classes and
     * their respective mappings of ASN.1 data types to Ruby objects.
     *
     * == Possible values for +tagging+
     *
     * When constructing an ASN1Data object the ASN.1 type definition may
     * require certain elements to be either implicitly or explicitly tagged.
     * This can be achieved by setting the +tagging+ attribute manually for
     * sub-classes of ASN1Data. Use the symbol +:IMPLICIT+ for implicit
     * tagging and +:EXPLICIT+ if the element requires explicit tagging.
     *
     * == Possible values for +tag_class+
     *
     * It is possible to create arbitrary ASN1Data objects that also support
     * a PRIVATE or APPLICATION tag class. Possible values for the +tag_class+
     * attribute are:
     * * +:UNIVERSAL+ (the default for untagged values)
     * * +:CONTEXT_SPECIFIC+ (the default for tagged values)
     * * +:APPLICATION+
     * * +:PRIVATE+
     *
     * == Tag constants
     *
     * There is a constant defined for each universal tag:
     * * OpenSSL::ASN1::EOC (0)
     * * OpenSSL::ASN1::BOOLEAN (1)
     * * OpenSSL::ASN1::INTEGER (2)
     * * OpenSSL::ASN1::BIT_STRING (3)
     * * OpenSSL::ASN1::OCTET_STRING (4)
     * * OpenSSL::ASN1::NULL (5)
     * * OpenSSL::ASN1::OBJECT (6)
     * * OpenSSL::ASN1::ENUMERATED (10)
     * * OpenSSL::ASN1::UTF8STRING (12)
     * * OpenSSL::ASN1::SEQUENCE (16)
     * * OpenSSL::ASN1::SET (17)
     * * OpenSSL::ASN1::NUMERICSTRING (18)
     * * OpenSSL::ASN1::PRINTABLESTRING (19)
     * * OpenSSL::ASN1::T61STRING (20)
     * * OpenSSL::ASN1::VIDEOTEXSTRING (21)
     * * OpenSSL::ASN1::IA5STRING (22)
     * * OpenSSL::ASN1::UTCTIME (23)
     * * OpenSSL::ASN1::GENERALIZEDTIME (24)
     * * OpenSSL::ASN1::GRAPHICSTRING (25)
     * * OpenSSL::ASN1::ISO64STRING (26)
     * * OpenSSL::ASN1::GENERALSTRING (27)
     * * OpenSSL::ASN1::UNIVERSALSTRING (28)
     * * OpenSSL::ASN1::BMPSTRING (30)
     *
     * == UNIVERSAL_TAG_NAME constant
     *
     * An Array that stores the name of a given tag number. These names are
     * the same as the name of the tag constant that is additionally defined,
     * e.g. UNIVERSAL_TAG_NAME[2] = "INTEGER" and OpenSSL::ASN1::INTEGER = 2.
     *
     * == Example usage
     *
     * === Decoding and viewing a DER-encoded file
     *   require 'openssl'
     *   require 'pp'
     *   der = File.binread('data.der')
     *   asn1 = OpenSSL::ASN1.decode(der)
     *   pp der
     *
     * === Creating an ASN.1 structure and DER-encoding it
     *   require 'openssl'
     *   version = OpenSSL::ASN1::Integer.new(1)
     *   # Explicitly 0-tagged implies context-specific tag class
     *   serial = OpenSSL::ASN1::Integer.new(12345, 0, :EXPLICIT, :CONTEXT_SPECIFIC)
     *   name = OpenSSL::ASN1::PrintableString.new('Data 1')
     *   sequence = OpenSSL::ASN1::Sequence.new( [ version, serial, name ] )
     *   der = sequence.to_der
     */
    mASN1 = rb_define_module_under(mOSSL, "ASN1");

    /* Document-class: OpenSSL::ASN1::ASN1Error
     *
     * Generic error class for all errors raised in ASN1 and any of the
     * classes defined in it.
     */
    eASN1Error = rb_define_class_under(mASN1, "ASN1Error", eOSSLError);
    rb_define_module_function(mASN1, "traverse", ossl_asn1_traverse, 1);
    rb_define_module_function(mASN1, "decode", ossl_asn1_decode, 1);
    rb_define_module_function(mASN1, "decode_all", ossl_asn1_decode_all, 1);
    ary = rb_ary_new();

    /*
     * Array storing tag names at the tag's index.
     */
    rb_define_const(mASN1, "UNIVERSAL_TAG_NAME", ary);
    for(i = 0; i < ossl_asn1_info_size; i++){
	if(ossl_asn1_info[i].name[0] == '[') continue;
	rb_define_const(mASN1, ossl_asn1_info[i].name, INT2NUM(i));
	rb_ary_store(ary, i, rb_str_new2(ossl_asn1_info[i].name));
    }

    /* Document-class: OpenSSL::ASN1::ASN1Data
     *
     * The top-level class representing any ASN.1 object. When parsed by
     * ASN1.decode, tagged values are always represented by an instance
     * of ASN1Data.
     *
     * == The role of ASN1Data for parsing tagged values
     *
     * When encoding an ASN.1 type it is inherently clear what original
     * type (e.g. INTEGER, OCTET STRING etc.) this value has, regardless
     * of its tagging.
     * But opposed to the time an ASN.1 type is to be encoded, when parsing
     * them it is not possible to deduce the "real type" of tagged
     * values. This is why tagged values are generally parsed into ASN1Data
     * instances, but with a different outcome for implicit and explicit
     * tagging.
     *
     * === Example of a parsed implicitly tagged value
     *
     * An implicitly 1-tagged INTEGER value will be parsed as an
     * ASN1Data with
     * * +tag+ equal to 1
     * * +tag_class+ equal to +:CONTEXT_SPECIFIC+
     * * +value+ equal to a +String+ that carries the raw encoding
     *   of the INTEGER.
     * This implies that a subsequent decoding step is required to
     * completely decode implicitly tagged values.
     *
     * === Example of a parsed explicitly tagged value
     *
     * An explicitly 1-tagged INTEGER value will be parsed as an
     * ASN1Data with
     * * +tag+ equal to 1
     * * +tag_class+ equal to +:CONTEXT_SPECIFIC+
     * * +value+ equal to an +Array+ with one single element, an
     *   instance of OpenSSL::ASN1::Integer, i.e. the inner element
     *   is the non-tagged primitive value, and the tagging is represented
     *   in the outer ASN1Data
     *
     * == Example - Decoding an implicitly tagged INTEGER
     *   int = OpenSSL::ASN1::Integer.new(1, 0, :IMPLICIT) # implicit 0-tagged
     *   seq = OpenSSL::ASN1::Sequence.new( [int] )
     *   der = seq.to_der
     *   asn1 = OpenSSL::ASN1.decode(der)
     *   # pp asn1 => #<OpenSSL::ASN1::Sequence:0x87326e0
     *   #              @infinite_length=false,
     *   #              @tag=16,
     *   #              @tag_class=:UNIVERSAL,
     *   #              @tagging=nil,
     *   #              @value=
     *   #                [#<OpenSSL::ASN1::ASN1Data:0x87326f4
     *   #                   @infinite_length=false,
     *   #                   @tag=0,
     *   #                   @tag_class=:CONTEXT_SPECIFIC,
     *   #                   @value="\x01">]>
     *   raw_int = asn1.value[0]
     *   # manually rewrite tag and tag class to make it an UNIVERSAL value
     *   raw_int.tag = OpenSSL::ASN1::INTEGER
     *   raw_int.tag_class = :UNIVERSAL
     *   int2 = OpenSSL::ASN1.decode(raw_int)
     *   puts int2.value # => 1
     *
     * == Example - Decoding an explicitly tagged INTEGER
     *   int = OpenSSL::ASN1::Integer.new(1, 0, :EXPLICIT) # explicit 0-tagged
     *   seq = OpenSSL::ASN1::Sequence.new( [int] )
     *   der = seq.to_der
     *   asn1 = OpenSSL::ASN1.decode(der)
     *   # pp asn1 => #<OpenSSL::ASN1::Sequence:0x87326e0
     *   #              @infinite_length=false,
     *   #              @tag=16,
     *   #              @tag_class=:UNIVERSAL,
     *   #              @tagging=nil,
     *   #              @value=
     *   #                [#<OpenSSL::ASN1::ASN1Data:0x87326f4
     *   #                   @infinite_length=false,
     *   #                   @tag=0,
     *   #                   @tag_class=:CONTEXT_SPECIFIC,
     *   #                   @value=
     *   #                     [#<OpenSSL::ASN1::Integer:0x85bf308
     *   #                        @infinite_length=false,
     *   #                        @tag=2,
     *   #                        @tag_class=:UNIVERSAL
     *   #                        @tagging=nil,
     *   #                        @value=1>]>]>
     *   int2 = asn1.value[0].value[0]
     *   puts int2.value # => 1
     */
    cASN1Data = rb_define_class_under(mASN1, "ASN1Data", rb_cObject);
    /*
     * Carries the value of a ASN.1 type.
     * Please confer Constructive and Primitive for the mappings between
     * ASN.1 data types and Ruby classes.
     */
    rb_attr(cASN1Data, rb_intern("value"), 1, 1, 0);
    /*
     * A +Number+ representing the tag number of this ASN1Data. Never +nil+.
     */
    rb_attr(cASN1Data, rb_intern("tag"), 1, 1, 0);
    /*
     * A +Symbol+ representing the tag class of this ASN1Data. Never +nil+.
     * See ASN1Data for possible values.
     */
    rb_attr(cASN1Data, rb_intern("tag_class"), 1, 1, 0);
    /*
     * Never +nil+. A +Boolean+ indicating whether the encoding was infinite
     * length (in the case of parsing) or whether an infinite length encoding
     * shall be used (in the encoding case).
     * In DER, every value has a finite length associated with it. But in
     * scenarios where large amounts of data need to be transferred it
     * might be desirable to have some kind of streaming support available.
     * For example, huge OCTET STRINGs are preferably sent in smaller-sized
     * chunks, each at a time.
     * This is possible in BER by setting the length bytes of an encoding
     * to zero and by this indicating that the following value will be
     * sent in chunks. Infinite length encodings are always constructed.
     * The end of such a stream of chunks is indicated by sending a EOC
     * (End of Content) tag. SETs and SEQUENCEs may use an infinite length
     * encoding, but also primitive types such as e.g. OCTET STRINGS or
     * BIT STRINGS may leverage this functionality (cf. ITU-T X.690).
     */
    rb_attr(cASN1Data, rb_intern("infinite_length"), 1, 1, 0);
    rb_define_method(cASN1Data, "initialize", ossl_asn1data_initialize, 3);
    rb_define_method(cASN1Data, "to_der", ossl_asn1data_to_der, 0);

    /* Document-class: OpenSSL::ASN1::Primitive
     *
     * The parent class for all primitive encodings. Attributes are the same as
     * for ASN1Data, with the addition of +tagging+.
     * Primitive values can never be infinite length encodings, thus it is not
     * possible to set the +infinite_length+ attribute for Primitive and its
     * sub-classes.
     *
     * == Primitive sub-classes and their mapping to Ruby classes
     * * OpenSSL::ASN1::EndOfContent    <=> +value+ is always +nil+
     * * OpenSSL::ASN1::Boolean         <=> +value+ is a +Boolean+
     * * OpenSSL::ASN1::Integer         <=> +value+ is a +Number+
     * * OpenSSL::ASN1::BitString       <=> +value+ is a +String+
     * * OpenSSL::ASN1::OctetString     <=> +value+ is a +String+
     * * OpenSSL::ASN1::Null            <=> +value+ is always +nil+
     * * OpenSSL::ASN1::Object          <=> +value+ is a +String+
     * * OpenSSL::ASN1::Enumerated      <=> +value+ is a +Number+
     * * OpenSSL::ASN1::UTF8String      <=> +value+ is a +String+
     * * OpenSSL::ASN1::NumericString   <=> +value+ is a +String+
     * * OpenSSL::ASN1::PrintableString <=> +value+ is a +String+
     * * OpenSSL::ASN1::T61String       <=> +value+ is a +String+
     * * OpenSSL::ASN1::VideotexString  <=> +value+ is a +String+
     * * OpenSSL::ASN1::IA5String       <=> +value+ is a +String+
     * * OpenSSL::ASN1::UTCTime         <=> +value+ is a +Time+
     * * OpenSSL::ASN1::GeneralizedTime <=> +value+ is a +Time+
     * * OpenSSL::ASN1::GraphicString   <=> +value+ is a +String+
     * * OpenSSL::ASN1::ISO64String     <=> +value+ is a +String+
     * * OpenSSL::ASN1::GeneralString   <=> +value+ is a +String+
     * * OpenSSL::ASN1::UniversalString <=> +value+ is a +String+
     * * OpenSSL::ASN1::BMPString       <=> +value+ is a +String+
     *
     * == OpenSSL::ASN1::BitString
     *
     * === Additional attributes
     * +unused_bits+: if the underlying BIT STRING's
     * length is a multiple of 8 then +unused_bits+ is 0. Otherwise
     * +unused_bits+ indicates the number of bits that are to be ignored in
     * the final octet of the +BitString+'s +value+.
     *
     * == OpenSSL::ASN1::ObjectId
     *
     * === Additional attributes
     * * +sn+: the short name as defined in <openssl/objects.h>.
     * * +ln+: the long name as defined in <openssl/objects.h>.
     * * +oid+: the object identifier as a +String+, e.g. "1.2.3.4.5"
     * * +short_name+: alias for +sn+.
     * * +long_name+: alias for +ln+.
     *
     * == Examples
     * With the Exception of OpenSSL::ASN1::EndOfContent, each Primitive class
     * constructor takes at least one parameter, the +value+.
     *
     * === Creating EndOfContent
     *   eoc = OpenSSL::ASN1::EndOfContent.new
     *
     * === Creating any other Primitive
     *   prim = <class>.new(value) # <class> being one of the sub-classes except EndOfContent
     *   prim_zero_tagged_implicit = <class>.new(value, 0, :IMPLICIT)
     *   prim_zero_tagged_explicit = <class>.new(value, 0, :EXPLICIT)
     */
    cASN1Primitive = rb_define_class_under(mASN1, "Primitive", cASN1Data);
    /*
     * May be used as a hint for encoding a value either implicitly or
     * explicitly by setting it either to +:IMPLICIT+ or to +:EXPLICIT+.
     * +tagging+ is not set when a ASN.1 structure is parsed using
     * OpenSSL::ASN1.decode.
     */
    rb_attr(cASN1Primitive, rb_intern("tagging"), 1, 1, Qtrue);
    rb_undef_method(cASN1Primitive, "infinite_length=");
    rb_define_method(cASN1Primitive, "initialize", ossl_asn1_initialize, -1);
    rb_define_method(cASN1Primitive, "to_der", ossl_asn1prim_to_der, 0);

    /* Document-class: OpenSSL::ASN1::Constructive
     *
     * The parent class for all constructed encodings. The +value+ attribute
     * of a Constructive is always an +Array+. Attributes are the same as
     * for ASN1Data, with the addition of +tagging+.
     *
     * == SET and SEQUENCE
     *
     * Most constructed encodings come in the form of a SET or a SEQUENCE.
     * These encodings are represented by one of the two sub-classes of
     * Constructive:
     * * OpenSSL::ASN1::Set
     * * OpenSSL::ASN1::Sequence
     * Please note that tagged sequences and sets are still parsed as
     * instances of ASN1Data. Find further details on tagged values
     * there.
     *
     * === Example - constructing a SEQUENCE
     *   int = OpenSSL::ASN1::Integer.new(1)
     *   str = OpenSSL::ASN1::PrintableString.new('abc')
     *   sequence = OpenSSL::ASN1::Sequence.new( [ int, str ] )
     *
     * === Example - constructing a SET
     *   int = OpenSSL::ASN1::Integer.new(1)
     *   str = OpenSSL::ASN1::PrintableString.new('abc')
     *   set = OpenSSL::ASN1::Set.new( [ int, str ] )
     *
     * == Infinite length primitive values
     *
     * The only case where Constructive is used directly is for infinite
     * length encodings of primitive values. These encodings are always
     * constructed, with the contents of the +value+ +Array+ being either
     * UNIVERSAL non-infinite length partial encodings of the actual value
     * or again constructive encodings with infinite length (i.e. infinite
     * length primitive encodings may be constructed recursively with another
     * infinite length value within an already infinite length value). Each
     * partial encoding must be of the same UNIVERSAL type as the overall
     * encoding. The value of the overall encoding consists of the
     * concatenation of each partial encoding taken in sequence. The +value+
     * array of the outer infinite length value must end with a
     * OpenSSL::ASN1::EndOfContent instance.
     *
     * Please note that it is not possible to encode Constructive without
     * the +infinite_length+ attribute being set to +true+, use
     * OpenSSL::ASN1::Sequence or OpenSSL::ASN1::Set in these cases instead.
     *
     * === Example - Infinite length OCTET STRING
     *   partial1 = OpenSSL::ASN1::OctetString.new("\x01")
     *   partial2 = OpenSSL::ASN1::OctetString.new("\x02")
     *   inf_octets = OpenSSL::ASN1::Constructive.new( [ partial1,
     *                                                   partial2,
     *                                                   OpenSSL::ASN1::EndOfContent.new ],
     *                                                 OpenSSL::ASN1::OCTET_STRING,
     *                                                 nil,
     *                                                 :UNIVERSAL )
     *   # The real value of inf_octets is "\x01\x02", i.e. the concatenation
     *   # of partial1 and partial2
     *   inf_octets.infinite_length = true
     *   der = inf_octets.to_der
     *   asn1 = OpenSSL::ASN1.decode(der)
     *   puts asn1.infinite_length # => true
     */
    cASN1Constructive = rb_define_class_under(mASN1,"Constructive", cASN1Data);
    rb_include_module(cASN1Constructive, rb_mEnumerable);
    /*
     * May be used as a hint for encoding a value either implicitly or
     * explicitly by setting it either to +:IMPLICIT+ or to +:EXPLICIT+.
     * +tagging+ is not set when a ASN.1 structure is parsed using
     * OpenSSL::ASN1.decode.
     */
    rb_attr(cASN1Constructive, rb_intern("tagging"), 1, 1, Qtrue);
    rb_define_method(cASN1Constructive, "initialize", ossl_asn1_initialize, -1);
    rb_define_method(cASN1Constructive, "to_der", ossl_asn1cons_to_der, 0);
    rb_define_method(cASN1Constructive, "each", ossl_asn1cons_each, 0);

#define OSSL_ASN1_DEFINE_CLASS(name, super) \
do{\
    cASN1##name = rb_define_class_under(mASN1, #name, cASN1##super);\
    rb_define_module_function(mASN1, #name, ossl_asn1_##name, -1);\
}while(0)

    OSSL_ASN1_DEFINE_CLASS(Boolean, Primitive);
    OSSL_ASN1_DEFINE_CLASS(Integer, Primitive);
    OSSL_ASN1_DEFINE_CLASS(Enumerated, Primitive);
    OSSL_ASN1_DEFINE_CLASS(BitString, Primitive);
    OSSL_ASN1_DEFINE_CLASS(OctetString, Primitive);
    OSSL_ASN1_DEFINE_CLASS(UTF8String, Primitive);
    OSSL_ASN1_DEFINE_CLASS(NumericString, Primitive);
    OSSL_ASN1_DEFINE_CLASS(PrintableString, Primitive);
    OSSL_ASN1_DEFINE_CLASS(T61String, Primitive);
    OSSL_ASN1_DEFINE_CLASS(VideotexString, Primitive);
    OSSL_ASN1_DEFINE_CLASS(IA5String, Primitive);
    OSSL_ASN1_DEFINE_CLASS(GraphicString, Primitive);
    OSSL_ASN1_DEFINE_CLASS(ISO64String, Primitive);
    OSSL_ASN1_DEFINE_CLASS(GeneralString, Primitive);
    OSSL_ASN1_DEFINE_CLASS(UniversalString, Primitive);
    OSSL_ASN1_DEFINE_CLASS(BMPString, Primitive);
    OSSL_ASN1_DEFINE_CLASS(Null, Primitive);
    OSSL_ASN1_DEFINE_CLASS(ObjectId, Primitive);
    OSSL_ASN1_DEFINE_CLASS(UTCTime, Primitive);
    OSSL_ASN1_DEFINE_CLASS(GeneralizedTime, Primitive);

    OSSL_ASN1_DEFINE_CLASS(Sequence, Constructive);
    OSSL_ASN1_DEFINE_CLASS(Set, Constructive);

    OSSL_ASN1_DEFINE_CLASS(EndOfContent, Data);


#if 0
    cASN1ObjectId = rb_define_class_under(mASN1, "ObjectId", cASN1Primitive);  /* let rdoc know */
#endif
    rb_define_singleton_method(cASN1ObjectId, "register", ossl_asn1obj_s_register, 3);
    rb_define_method(cASN1ObjectId, "sn", ossl_asn1obj_get_sn, 0);
    rb_define_method(cASN1ObjectId, "ln", ossl_asn1obj_get_ln, 0);
    rb_define_method(cASN1ObjectId, "oid", ossl_asn1obj_get_oid, 0);
    rb_define_alias(cASN1ObjectId, "short_name", "sn");
    rb_define_alias(cASN1ObjectId, "long_name", "ln");
    rb_attr(cASN1BitString, rb_intern("unused_bits"), 1, 1, 0);

    rb_define_method(cASN1EndOfContent, "initialize", ossl_asn1eoc_initialize, 0);

    class_tag_map = rb_hash_new();
    rb_hash_aset(class_tag_map, cASN1EndOfContent, INT2NUM(V_ASN1_EOC));
    rb_hash_aset(class_tag_map, cASN1Boolean, INT2NUM(V_ASN1_BOOLEAN));
    rb_hash_aset(class_tag_map, cASN1Integer, INT2NUM(V_ASN1_INTEGER));
    rb_hash_aset(class_tag_map, cASN1BitString, INT2NUM(V_ASN1_BIT_STRING));
    rb_hash_aset(class_tag_map, cASN1OctetString, INT2NUM(V_ASN1_OCTET_STRING));
    rb_hash_aset(class_tag_map, cASN1Null, INT2NUM(V_ASN1_NULL));
    rb_hash_aset(class_tag_map, cASN1ObjectId, INT2NUM(V_ASN1_OBJECT));
    rb_hash_aset(class_tag_map, cASN1Enumerated, INT2NUM(V_ASN1_ENUMERATED));
    rb_hash_aset(class_tag_map, cASN1UTF8String, INT2NUM(V_ASN1_UTF8STRING));
    rb_hash_aset(class_tag_map, cASN1Sequence, INT2NUM(V_ASN1_SEQUENCE));
    rb_hash_aset(class_tag_map, cASN1Set, INT2NUM(V_ASN1_SET));
    rb_hash_aset(class_tag_map, cASN1NumericString, INT2NUM(V_ASN1_NUMERICSTRING));
    rb_hash_aset(class_tag_map, cASN1PrintableString, INT2NUM(V_ASN1_PRINTABLESTRING));
    rb_hash_aset(class_tag_map, cASN1T61String, INT2NUM(V_ASN1_T61STRING));
    rb_hash_aset(class_tag_map, cASN1VideotexString, INT2NUM(V_ASN1_VIDEOTEXSTRING));
    rb_hash_aset(class_tag_map, cASN1IA5String, INT2NUM(V_ASN1_IA5STRING));
    rb_hash_aset(class_tag_map, cASN1UTCTime, INT2NUM(V_ASN1_UTCTIME));
    rb_hash_aset(class_tag_map, cASN1GeneralizedTime, INT2NUM(V_ASN1_GENERALIZEDTIME));
    rb_hash_aset(class_tag_map, cASN1GraphicString, INT2NUM(V_ASN1_GRAPHICSTRING));
    rb_hash_aset(class_tag_map, cASN1ISO64String, INT2NUM(V_ASN1_ISO64STRING));
    rb_hash_aset(class_tag_map, cASN1GeneralString, INT2NUM(V_ASN1_GENERALSTRING));
    rb_hash_aset(class_tag_map, cASN1UniversalString, INT2NUM(V_ASN1_UNIVERSALSTRING));
    rb_hash_aset(class_tag_map, cASN1BMPString, INT2NUM(V_ASN1_BMPSTRING));
    rb_global_variable(&class_tag_map);
}

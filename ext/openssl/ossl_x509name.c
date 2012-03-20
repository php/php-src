/*
 * $Id$
 * 'OpenSSL for Ruby' project
 * Copyright (C) 2001 Michal Rokos <m.rokos@sh.cvut.cz>
 * All rights reserved.
 */
/*
 * This program is licenced under the same licence as Ruby.
 * (See the file 'LICENCE'.)
 */
#include "ossl.h"

#define WrapX509Name(klass, obj, name) do { \
    if (!(name)) { \
	ossl_raise(rb_eRuntimeError, "Name wasn't initialized."); \
    } \
    (obj) = Data_Wrap_Struct((klass), 0, X509_NAME_free, (name)); \
} while (0)
#define GetX509Name(obj, name) do { \
    Data_Get_Struct((obj), X509_NAME, (name)); \
    if (!(name)) { \
	ossl_raise(rb_eRuntimeError, "Name wasn't initialized."); \
    } \
} while (0)
#define SafeGetX509Name(obj, name) do { \
    OSSL_Check_Kind((obj), cX509Name); \
    GetX509Name((obj), (name)); \
} while (0)

#define OBJECT_TYPE_TEMPLATE \
  rb_const_get(cX509Name, rb_intern("OBJECT_TYPE_TEMPLATE"))
#define DEFAULT_OBJECT_TYPE \
  rb_const_get(cX509Name, rb_intern("DEFAULT_OBJECT_TYPE"))

/*
 * Classes
 */
VALUE cX509Name;
VALUE eX509NameError;

/*
 * Public
 */
VALUE
ossl_x509name_new(X509_NAME *name)
{
    X509_NAME *new;
    VALUE obj;

    if (!name) {
	new = X509_NAME_new();
    } else {
	new = X509_NAME_dup(name);
    }
    if (!new) {
	ossl_raise(eX509NameError, NULL);
    }
    WrapX509Name(cX509Name, obj, new);

    return obj;
}

X509_NAME *
GetX509NamePtr(VALUE obj)
{
    X509_NAME *name;

    SafeGetX509Name(obj, name);

    return name;
}

/*
 * Private
 */
static VALUE
ossl_x509name_alloc(VALUE klass)
{
    X509_NAME *name;
    VALUE obj;

    if (!(name = X509_NAME_new())) {
	ossl_raise(eX509NameError, NULL);
    }
    WrapX509Name(klass, obj, name);

    return obj;
}

static ID id_aref;
static VALUE ossl_x509name_add_entry(int, VALUE*, VALUE);
#define rb_aref(obj, key) rb_funcall((obj), id_aref, 1, (key))

static VALUE
ossl_x509name_init_i(VALUE i, VALUE args)
{
    VALUE self = rb_ary_entry(args, 0);
    VALUE template = rb_ary_entry(args, 1);
    VALUE entry[3];

    Check_Type(i, T_ARRAY);
    entry[0] = rb_ary_entry(i, 0);
    entry[1] = rb_ary_entry(i, 1);
    entry[2] = rb_ary_entry(i, 2);
    if(NIL_P(entry[2])) entry[2] = rb_aref(template, entry[0]);
    if(NIL_P(entry[2])) entry[2] = DEFAULT_OBJECT_TYPE;
    ossl_x509name_add_entry(3, entry, self);

    return Qnil;
}

/*
 * call-seq:
 *    X509::Name.new => name
 *    X509::Name.new(string) => name
 *    X509::Name.new(dn) => name
 *    X509::Name.new(dn, template) => name
 */
static VALUE
ossl_x509name_initialize(int argc, VALUE *argv, VALUE self)
{
    X509_NAME *name;
    VALUE arg, template;

    GetX509Name(self, name);
    if (rb_scan_args(argc, argv, "02", &arg, &template) == 0) {
	return self;
    }
    else {
	VALUE tmp = rb_check_array_type(arg);
	if (!NIL_P(tmp)) {
	    VALUE args;
	    if(NIL_P(template)) template = OBJECT_TYPE_TEMPLATE;
	    args = rb_ary_new3(2, self, template);
	    rb_block_call(tmp, rb_intern("each"), 0, 0, ossl_x509name_init_i, args);
	}
	else{
	    const unsigned char *p;
	    VALUE str = ossl_to_der_if_possible(arg);
	    X509_NAME *x;
	    StringValue(str);
	    p = (unsigned char *)RSTRING_PTR(str);
	    x = d2i_X509_NAME(&name, &p, RSTRING_LEN(str));
	    DATA_PTR(self) = name;
	    if(!x){
		ossl_raise(eX509NameError, NULL);
	    }
	}
    }

    return self;
}

/*
 * call-seq:
 *    name.add_entry(oid, value [, type]) => self
 */
static
VALUE ossl_x509name_add_entry(int argc, VALUE *argv, VALUE self)
{
    X509_NAME *name;
    VALUE oid, value, type;

    rb_scan_args(argc, argv, "21", &oid, &value, &type);
    StringValue(oid);
    StringValue(value);
    if(NIL_P(type)) type = rb_aref(OBJECT_TYPE_TEMPLATE, oid);
    GetX509Name(self, name);
    if (!X509_NAME_add_entry_by_txt(name, RSTRING_PTR(oid), NUM2INT(type),
		(const unsigned char *)RSTRING_PTR(value), RSTRING_LENINT(value), -1, 0)) {
	ossl_raise(eX509NameError, NULL);
    }

    return self;
}

static VALUE
ossl_x509name_to_s_old(VALUE self)
{
    X509_NAME *name;
    char *buf;
    VALUE str;

    GetX509Name(self, name);
    buf = X509_NAME_oneline(name, NULL, 0);
    str = rb_str_new2(buf);
    OPENSSL_free(buf);

    return str;
}

/*
 * call-seq:
 *    name.to_s => string
 *    name.to_s(integer) => string
 */
static VALUE
ossl_x509name_to_s(int argc, VALUE *argv, VALUE self)
{
    X509_NAME *name;
    VALUE flag, str;
    BIO *out;
    unsigned long iflag;

    rb_scan_args(argc, argv, "01", &flag);
    if (NIL_P(flag))
	return ossl_x509name_to_s_old(self);
    else iflag = NUM2ULONG(flag);
    if (!(out = BIO_new(BIO_s_mem())))
	ossl_raise(eX509NameError, NULL);
    GetX509Name(self, name);
    if (!X509_NAME_print_ex(out, name, 0, iflag)){
	BIO_free(out);
	ossl_raise(eX509NameError, NULL);
    }
    str = ossl_membio2str(out);

    return str;
}

/*
 * call-seq:
 *    name.to_a => [[name, data, type], ...]
 */
static VALUE
ossl_x509name_to_a(VALUE self)
{
    X509_NAME *name;
    X509_NAME_ENTRY *entry;
    int i,entries,nid;
    char long_name[512];
    const char *short_name;
    VALUE ary, vname, ret;

    GetX509Name(self, name);
    entries = X509_NAME_entry_count(name);
    if (entries < 0) {
	OSSL_Debug("name entries < 0!");
	return rb_ary_new();
    }
    ret = rb_ary_new2(entries);
    for (i=0; i<entries; i++) {
	if (!(entry = X509_NAME_get_entry(name, i))) {
	    ossl_raise(eX509NameError, NULL);
	}
	if (!i2t_ASN1_OBJECT(long_name, sizeof(long_name), entry->object)) {
	    ossl_raise(eX509NameError, NULL);
	}
	nid = OBJ_ln2nid(long_name);
	if (nid == NID_undef) {
	    vname = rb_str_new2((const char *) &long_name);
	} else {
	    short_name = OBJ_nid2sn(nid);
	    vname = rb_str_new2(short_name); /*do not free*/
	}
	ary = rb_ary_new3(3,
			  vname,
        		  rb_str_new((const char *)entry->value->data, entry->value->length),
        		  INT2FIX(entry->value->type));
	rb_ary_push(ret, ary);
    }
    return ret;
}

static int
ossl_x509name_cmp0(VALUE self, VALUE other)
{
    X509_NAME *name1, *name2;

    GetX509Name(self, name1);
    SafeGetX509Name(other, name2);

    return X509_NAME_cmp(name1, name2);
}

/*
 * call-seq:
 *    name.cmp other => integer
 *    name.<=> other => integer
 *
 * Compares this Name with +other+ and returns 0 if they are the same and -1 or
 * +1 if they are greater or less than each other respectively.
 */
static VALUE
ossl_x509name_cmp(VALUE self, VALUE other)
{
    int result;

    result = ossl_x509name_cmp0(self, other);
    if (result < 0) return INT2FIX(-1);
    if (result > 1) return INT2FIX(1);

    return INT2FIX(0);
}

static VALUE
ossl_x509name_eql(VALUE self, VALUE other)
{
    int result;

    if(CLASS_OF(other) != cX509Name) return Qfalse;
    result = ossl_x509name_cmp0(self, other);

    return (result == 0) ? Qtrue : Qfalse;
}

/*
 * call-seq:
 *    name.hash => integer
 *
 * The hash value returned is suitable for use as a certificate's filename in
 * a CA path.
 */
static VALUE
ossl_x509name_hash(VALUE self)
{
    X509_NAME *name;
    unsigned long hash;

    GetX509Name(self, name);

    hash = X509_NAME_hash(name);

    return ULONG2NUM(hash);
}

#ifdef HAVE_X509_NAME_HASH_OLD
/*
 * call-seq:
 *    name.hash_old => integer
 *
 * hash_old returns MD5 based hash used in OpenSSL 0.9.X.
 */
static VALUE
ossl_x509name_hash_old(VALUE self)
{
    X509_NAME *name;
    unsigned long hash;

    GetX509Name(self, name);

    hash = X509_NAME_hash_old(name);

    return ULONG2NUM(hash);
}
#endif

/*
 * call-seq:
 *    name.to_der => string
 */
static VALUE
ossl_x509name_to_der(VALUE self)
{
    X509_NAME *name;
    VALUE str;
    long len;
    unsigned char *p;

    GetX509Name(self, name);
    if((len = i2d_X509_NAME(name, NULL)) <= 0)
	ossl_raise(eX509NameError, NULL);
    str = rb_str_new(0, len);
    p = (unsigned char *)RSTRING_PTR(str);
    if(i2d_X509_NAME(name, &p) <= 0)
	ossl_raise(eX509NameError, NULL);
    ossl_str_adjust(str, p);

    return str;
}

/*
 * INIT
 */
void
Init_ossl_x509name()
{
    VALUE utf8str, ptrstr, ia5str, hash;

    id_aref = rb_intern("[]");
    eX509NameError = rb_define_class_under(mX509, "NameError", eOSSLError);
    cX509Name = rb_define_class_under(mX509, "Name", rb_cObject);

    rb_include_module(cX509Name, rb_mComparable);

    rb_define_alloc_func(cX509Name, ossl_x509name_alloc);
    rb_define_method(cX509Name, "initialize", ossl_x509name_initialize, -1);
    rb_define_method(cX509Name, "add_entry", ossl_x509name_add_entry, -1);
    rb_define_method(cX509Name, "to_s", ossl_x509name_to_s, -1);
    rb_define_method(cX509Name, "to_a", ossl_x509name_to_a, 0);
    rb_define_method(cX509Name, "cmp", ossl_x509name_cmp, 1);
    rb_define_alias(cX509Name, "<=>", "cmp");
    rb_define_method(cX509Name, "eql?", ossl_x509name_eql, 1);
    rb_define_method(cX509Name, "hash", ossl_x509name_hash, 0);
#ifdef HAVE_X509_NAME_HASH_OLD
    rb_define_method(cX509Name, "hash_old", ossl_x509name_hash_old, 0);
#endif
    rb_define_method(cX509Name, "to_der", ossl_x509name_to_der, 0);

    utf8str = INT2NUM(V_ASN1_UTF8STRING);
    ptrstr = INT2NUM(V_ASN1_PRINTABLESTRING);
    ia5str = INT2NUM(V_ASN1_IA5STRING);
    rb_define_const(cX509Name, "DEFAULT_OBJECT_TYPE", utf8str);
    hash = rb_hash_new();
    RHASH(hash)->ifnone = utf8str;
    rb_hash_aset(hash, rb_str_new2("C"), ptrstr);
    rb_hash_aset(hash, rb_str_new2("countryName"), ptrstr);
    rb_hash_aset(hash, rb_str_new2("serialNumber"), ptrstr);
    rb_hash_aset(hash, rb_str_new2("dnQualifier"), ptrstr);
    rb_hash_aset(hash, rb_str_new2("DC"), ia5str);
    rb_hash_aset(hash, rb_str_new2("domainComponent"), ia5str);
    rb_hash_aset(hash, rb_str_new2("emailAddress"), ia5str);
    rb_define_const(cX509Name, "OBJECT_TYPE_TEMPLATE", hash);

    rb_define_const(cX509Name, "COMPAT", ULONG2NUM(XN_FLAG_COMPAT));
    rb_define_const(cX509Name, "RFC2253", ULONG2NUM(XN_FLAG_RFC2253));
    rb_define_const(cX509Name, "ONELINE", ULONG2NUM(XN_FLAG_ONELINE));
    rb_define_const(cX509Name, "MULTILINE", ULONG2NUM(XN_FLAG_MULTILINE));
}

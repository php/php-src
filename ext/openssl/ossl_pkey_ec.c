/*
 * Copyright (C) 2006-2007 Technorama Ltd. <oss-ruby@technorama.net>
 */

#include "ossl.h"

#if !defined(OPENSSL_NO_EC) && (OPENSSL_VERSION_NUMBER >= 0x0090802fL)

typedef struct {
	EC_GROUP *group;
	int dont_free;
} ossl_ec_group;

typedef struct {
	EC_POINT *point;
	int dont_free;
} ossl_ec_point;


#define EXPORT_PEM 0
#define EXPORT_DER 1


#define GetPKeyEC(obj, pkey) do { \
    GetPKey((obj), (pkey)); \
    if (EVP_PKEY_type((pkey)->type) != EVP_PKEY_EC) { \
	ossl_raise(rb_eRuntimeError, "THIS IS NOT A EC PKEY!"); \
    } \
} while (0)

#define SafeGet_ec_group(obj, group) do { \
    OSSL_Check_Kind((obj), cEC_GROUP); \
    Data_Get_Struct((obj), ossl_ec_group, (group)); \
} while(0)

#define Get_EC_KEY(obj, key) do { \
    EVP_PKEY *pkey; \
    GetPKeyEC((obj), pkey); \
    (key) = pkey->pkey.ec; \
} while(0)

#define Require_EC_KEY(obj, key) do { \
    Get_EC_KEY((obj), (key)); \
    if ((key) == NULL) \
        ossl_raise(eECError, "EC_KEY is not initialized"); \
} while(0)

#define SafeRequire_EC_KEY(obj, key) do { \
    OSSL_Check_Kind((obj), cEC); \
    Require_EC_KEY((obj), (key)); \
} while (0)

#define Get_EC_GROUP(obj, g) do { \
    ossl_ec_group *ec_group; \
    Data_Get_Struct((obj), ossl_ec_group, ec_group); \
    if (ec_group == NULL) \
        ossl_raise(eEC_GROUP, "missing ossl_ec_group structure"); \
    (g) = ec_group->group; \
} while(0)

#define Require_EC_GROUP(obj, group) do { \
    Get_EC_GROUP((obj), (group)); \
    if ((group) == NULL) \
        ossl_raise(eEC_GROUP, "EC_GROUP is not initialized"); \
} while(0)

#define SafeRequire_EC_GROUP(obj, group) do { \
    OSSL_Check_Kind((obj), cEC_GROUP); \
    Require_EC_GROUP((obj), (group)); \
} while(0)

#define Get_EC_POINT(obj, p) do { \
    ossl_ec_point *ec_point; \
    Data_Get_Struct((obj), ossl_ec_point, ec_point); \
    if (ec_point == NULL) \
        ossl_raise(eEC_POINT, "missing ossl_ec_point structure"); \
    (p) = ec_point->point; \
} while(0)

#define Require_EC_POINT(obj, point) do { \
    Get_EC_POINT((obj), (point)); \
    if ((point) == NULL) \
        ossl_raise(eEC_POINT, "EC_POINT is not initialized"); \
} while(0)

#define SafeRequire_EC_POINT(obj, point) do { \
    OSSL_Check_Kind((obj), cEC_POINT); \
    Require_EC_POINT((obj), (point)); \
} while(0)

VALUE cEC;
VALUE eECError;
VALUE cEC_GROUP;
VALUE eEC_GROUP;
VALUE cEC_POINT;
VALUE eEC_POINT;

static ID s_GFp;
static ID s_GFp_simple;
static ID s_GFp_mont;
static ID s_GFp_nist;
static ID s_GF2m;
static ID s_GF2m_simple;

static ID ID_uncompressed;
static ID ID_compressed;
static ID ID_hybrid;

static VALUE ec_instance(VALUE klass, EC_KEY *ec)
{
    EVP_PKEY *pkey;
    VALUE obj;

    if (!ec) {
	return Qfalse;
    }
    if (!(pkey = EVP_PKEY_new())) {
	return Qfalse;
    }
    if (!EVP_PKEY_assign_EC_KEY(pkey, ec)) {
	EVP_PKEY_free(pkey);
	return Qfalse;
    }
    WrapPKey(klass, obj, pkey);

    return obj;
}

VALUE ossl_ec_new(EVP_PKEY *pkey)
{
    VALUE obj;

    if (!pkey) {
	obj = ec_instance(cEC, EC_KEY_new());
    } else {
	if (EVP_PKEY_type(pkey->type) != EVP_PKEY_EC) {
	    ossl_raise(rb_eTypeError, "Not a EC key!");
	}
	WrapPKey(cEC, obj, pkey);
    }
    if (obj == Qfalse) {
	ossl_raise(eECError, NULL);
    }

    return obj;
}


/*  call-seq:
 *     OpenSSL::PKey::EC.new()
 *     OpenSSL::PKey::EC.new(ec_key)
 *     OpenSSL::PKey::EC.new(ec_group)
 *     OpenSSL::PKey::EC.new("secp112r1")
 *     OpenSSL::PKey::EC.new(pem_string)
 *     OpenSSL::PKey::EC.new(pem_string [, pwd])
 *     OpenSSL::PKey::EC.new(der_string)
 *
 *  See the OpenSSL documentation for:
 *     EC_KEY_*
 */
static VALUE ossl_ec_key_initialize(int argc, VALUE *argv, VALUE self)
{
    EVP_PKEY *pkey;
    EC_KEY *ec = NULL;
    VALUE arg, pass;
    VALUE group = Qnil;
    char *passwd = NULL;

    GetPKey(self, pkey);
    if (pkey->pkey.ec)
        ossl_raise(eECError, "EC_KEY already initialized");

    rb_scan_args(argc, argv, "02", &arg, &pass);

    if (NIL_P(arg)) {
        ec = EC_KEY_new();
    } else {
        if (rb_obj_is_kind_of(arg, cEC)) {
            EC_KEY *other_ec = NULL;

            SafeRequire_EC_KEY(arg, other_ec);
            ec = EC_KEY_dup(other_ec);
        } else if (rb_obj_is_kind_of(arg, cEC_GROUP)) {
        	ec = EC_KEY_new();
        	group = arg;
        } else {
            BIO *in = ossl_obj2bio(arg);

            if (!NIL_P(pass)) {
		passwd = StringValuePtr(pass);
	    }
	    ec = PEM_read_bio_ECPrivateKey(in, NULL, ossl_pem_passwd_cb, passwd);
            if (!ec) {
		OSSL_BIO_reset(in);
		ec = PEM_read_bio_EC_PUBKEY(in, NULL, ossl_pem_passwd_cb, passwd);
            }
            if (!ec) {
		OSSL_BIO_reset(in);
                ec = d2i_ECPrivateKey_bio(in, NULL);
            }
            if (!ec) {
		OSSL_BIO_reset(in);
                ec = d2i_EC_PUBKEY_bio(in, NULL);
            }

            BIO_free(in);

            if (ec == NULL) {
                const char *name = StringValueCStr(arg);
                int nid = OBJ_sn2nid(name);

                (void)ERR_get_error();
                if (nid == NID_undef)
                    ossl_raise(eECError, "unknown curve name (%s)\n", name);

                if ((ec = EC_KEY_new_by_curve_name(nid)) == NULL)
                    ossl_raise(eECError, "unable to create curve (%s)\n", name);

                EC_KEY_set_asn1_flag(ec, OPENSSL_EC_NAMED_CURVE);
                EC_KEY_set_conv_form(ec, POINT_CONVERSION_UNCOMPRESSED);
            }
        }
    }

    if (ec == NULL)
        ossl_raise(eECError, NULL);

    if (!EVP_PKEY_assign_EC_KEY(pkey, ec)) {
	EC_KEY_free(ec);
	ossl_raise(eECError, "EVP_PKEY_assign_EC_KEY");
    }

    rb_iv_set(self, "@group", Qnil);

    if (!NIL_P(group))
        rb_funcall(self, rb_intern("group="), 1, arg);

    return self;
}

/*
 *  call-seq:
 *     key.group   => group
 *
 *  Returns a constant <code>OpenSSL::EC::Group</code> that is tied to the key.
 *  Modifying the returned group can make the key invalid.
 */
static VALUE ossl_ec_key_get_group(VALUE self)
{
    VALUE group_v;
    EC_KEY *ec;
    ossl_ec_group *ec_group;
    EC_GROUP *group;

    Require_EC_KEY(self, ec);

    group_v = rb_iv_get(self, "@group");
    if (!NIL_P(group_v))
        return group_v;

    if ((group = (EC_GROUP *)EC_KEY_get0_group(ec)) != NULL) {
        group_v = rb_obj_alloc(cEC_GROUP);
        SafeGet_ec_group(group_v, ec_group);
        ec_group->group = group;
        ec_group->dont_free = 1;
        rb_iv_set(group_v, "@key", self);
        rb_iv_set(self, "@group", group_v);
        return group_v;
    }

    return Qnil;
}

/*
 *  call-seq:
 *     key.group = group   => group
 *
 *  Returns the same object passed, not the group object associated with the key.
 *  If you wish to access the group object tied to the key call key.group after setting
 *  the group.
 *
 *  Setting the group will immediately destroy any previously assigned group object.
 *  The group is internally copied by OpenSSL.  Modifying the original group after
 *  assignment will not effect the internal key structure.
 *  (your changes may be lost).  BE CAREFUL.
 *
 *  EC_KEY_set_group calls EC_GROUP_free(key->group) then EC_GROUP_dup(), not EC_GROUP_copy.
 *  This documentation is accurate for OpenSSL 0.9.8b.
 */
static VALUE ossl_ec_key_set_group(VALUE self, VALUE group_v)
{
    VALUE old_group_v;
    EC_KEY *ec;
    EC_GROUP *group;

    Require_EC_KEY(self, ec);
    SafeRequire_EC_GROUP(group_v, group);

    old_group_v = rb_iv_get(self, "@group");
    if (!NIL_P(old_group_v)) {
        ossl_ec_group *old_ec_group;
        SafeGet_ec_group(old_group_v, old_ec_group);

        old_ec_group->group = NULL;
        old_ec_group->dont_free = 0;
        rb_iv_set(old_group_v, "@key", Qnil);
    }

    rb_iv_set(self, "@group", Qnil);

    if (EC_KEY_set_group(ec, group) != 1)
        ossl_raise(eECError, "EC_KEY_set_group");

    return group_v;
}

/*
 *  call-seq:
 *     key.private_key   => OpenSSL::BN
 *
 *  See the OpenSSL documentation for EC_KEY_get0_private_key()
 */
static VALUE ossl_ec_key_get_private_key(VALUE self)
{
    EC_KEY *ec;
    const BIGNUM *bn;

    Require_EC_KEY(self, ec);

    if ((bn = EC_KEY_get0_private_key(ec)) == NULL)
        return Qnil;

    return ossl_bn_new(bn);
}

/*
 *  call-seq:
 *     key.private_key = openssl_bn
 *
 *  See the OpenSSL documentation for EC_KEY_set_private_key()
 */
static VALUE ossl_ec_key_set_private_key(VALUE self, VALUE private_key)
{
    EC_KEY *ec;
    BIGNUM *bn = NULL;

    Require_EC_KEY(self, ec);
    if (!NIL_P(private_key))
        bn = GetBNPtr(private_key);

    switch (EC_KEY_set_private_key(ec, bn)) {
    case 1:
        break;
    case 0:
        if (bn == NULL)
            break;
    default:
        ossl_raise(eECError, "EC_KEY_set_private_key");
    }

    return private_key;
}


static VALUE ossl_ec_point_dup(const EC_POINT *point, VALUE group_v)
{
    VALUE obj;
    const EC_GROUP *group;
    ossl_ec_point *new_point;

    obj = rb_obj_alloc(cEC_POINT);
    Data_Get_Struct(obj, ossl_ec_point, new_point);

    SafeRequire_EC_GROUP(group_v, group);

    new_point->point = EC_POINT_dup(point, group);
    if (new_point->point == NULL)
        ossl_raise(eEC_POINT, "EC_POINT_dup");
    rb_iv_set(obj, "@group", group_v);

    return obj;
}

/*
 *  call-seq:
 *     key.public_key   => OpenSSL::PKey::EC::Point
 *
 *  See the OpenSSL documentation for EC_KEY_get0_public_key()
 */
static VALUE ossl_ec_key_get_public_key(VALUE self)
{
    EC_KEY *ec;
    const EC_POINT *point;
    VALUE group;

    Require_EC_KEY(self, ec);

    if ((point = EC_KEY_get0_public_key(ec)) == NULL)
        return Qnil;

    group = rb_funcall(self, rb_intern("group"), 0);
    if (NIL_P(group))
        ossl_raise(eECError, "EC_KEY_get0_get0_group (has public_key but no group???");

    return ossl_ec_point_dup(point, group);
}

/*
 *  call-seq:
 *     key.public_key = ec_point
 *
 *  See the OpenSSL documentation for EC_KEY_set_public_key()
 */
static VALUE ossl_ec_key_set_public_key(VALUE self, VALUE public_key)
{
    EC_KEY *ec;
    EC_POINT *point = NULL;

    Require_EC_KEY(self, ec);
    if (!NIL_P(public_key))
        SafeRequire_EC_POINT(public_key, point);

    switch (EC_KEY_set_public_key(ec, point)) {
    case 1:
        break;
    case 0:
        if (point == NULL)
            break;
    default:
        ossl_raise(eECError, "EC_KEY_set_public_key");
    }

    return public_key;
}

/*
 *  call-seq:
 *     key.public_key? => true or false
 *
 *  Both public_key? and private_key? may return false at the same time unlike other PKey classes.
 */
static VALUE ossl_ec_key_is_public_key(VALUE self)
{
    EC_KEY *ec;

    Require_EC_KEY(self, ec);

    return (EC_KEY_get0_public_key(ec) ? Qtrue : Qfalse);
}

/*
 *  call-seq:
 *     key.private_key? => true or false
 *
 *  Both public_key? and private_key? may return false at the same time unlike other PKey classes.
 */
static VALUE ossl_ec_key_is_private_key(VALUE self)
{
    EC_KEY *ec;

    Require_EC_KEY(self, ec);

    return (EC_KEY_get0_private_key(ec) ? Qtrue : Qfalse);
}

static VALUE ossl_ec_key_to_string(VALUE self, VALUE ciph, VALUE pass, int format)
{
    EC_KEY *ec;
    BIO *out;
    int i = -1;
    int private = 0;
    char *password = NULL;
    VALUE str;

    Require_EC_KEY(self, ec);

    if (EC_KEY_get0_public_key(ec) == NULL)
        ossl_raise(eECError, "can't export - no public key set");

    if (EC_KEY_check_key(ec) != 1)
	ossl_raise(eECError, "can't export - EC_KEY_check_key failed");

    if (EC_KEY_get0_private_key(ec))
        private = 1;

    if (!(out = BIO_new(BIO_s_mem())))
        ossl_raise(eECError, "BIO_new(BIO_s_mem())");

    switch(format) {
    case EXPORT_PEM:
    	if (private) {
	    const EVP_CIPHER *cipher;
	    if (!NIL_P(ciph)) {
		cipher = GetCipherPtr(ciph);
		if (!NIL_P(pass)) {
		    password = StringValuePtr(pass);
		}
	    }
	    else {
		cipher = NULL;
	    }
            i = PEM_write_bio_ECPrivateKey(out, ec, cipher, NULL, 0, NULL, password);
    	} else {
            i = PEM_write_bio_EC_PUBKEY(out, ec);
        }

    	break;
    case EXPORT_DER:
        if (private) {
            i = i2d_ECPrivateKey_bio(out, ec);
        } else {
            i = i2d_EC_PUBKEY_bio(out, ec);
        }

    	break;
    default:
        BIO_free(out);
    	ossl_raise(rb_eRuntimeError, "unknown format (internal error)");
    }

    if (i != 1) {
        BIO_free(out);
        ossl_raise(eECError, "outlen=%d", i);
    }

    str = ossl_membio2str(out);

    return str;
}

/*
 *  call-seq:
 *     key.to_pem   => String
 *     key.to_pem(cipher, pass_phrase) => String
 *
 * Outputs the EC key in PEM encoding.  If +cipher+ and +pass_phrase+ are
 * given they will be used to encrypt the key.  +cipher+ must be an
 * OpenSSL::Cipher::Cipher instance. Note that encryption will only be
 * effective for a private key, public keys will always be encoded in plain
 * text.
 *
 */
static VALUE ossl_ec_key_to_pem(int argc, VALUE *argv, VALUE self)
{
    VALUE cipher, passwd;
    rb_scan_args(argc, argv, "02", &cipher, &passwd);
    return ossl_ec_key_to_string(self, cipher, passwd, EXPORT_PEM);
}

/*
 *  call-seq:
 *     key.to_der   => String
 *
 *  See the OpenSSL documentation for i2d_ECPrivateKey_bio()
 */
static VALUE ossl_ec_key_to_der(VALUE self)
{
    return ossl_ec_key_to_string(self, Qnil, Qnil, EXPORT_DER);
}

/*
 *  call-seq:
 *     key.to_text   => String
 *
 *  See the OpenSSL documentation for EC_KEY_print()
 */
static VALUE ossl_ec_key_to_text(VALUE self)
{
    EC_KEY *ec;
    BIO *out;
    VALUE str;

    Require_EC_KEY(self, ec);
    if (!(out = BIO_new(BIO_s_mem()))) {
	ossl_raise(eECError, "BIO_new(BIO_s_mem())");
    }
    if (!EC_KEY_print(out, ec, 0)) {
	BIO_free(out);
	ossl_raise(eECError, "EC_KEY_print");
    }
    str = ossl_membio2str(out);

    return str;
}

/*
 *  call-seq:
 *     key.generate_key   => self
 *
 *  See the OpenSSL documentation for EC_KEY_generate_key()
 */
static VALUE ossl_ec_key_generate_key(VALUE self)
{
    EC_KEY *ec;

    Require_EC_KEY(self, ec);

    if (EC_KEY_generate_key(ec) != 1)
	ossl_raise(eECError, "EC_KEY_generate_key");

    return self;
}

/*
 *  call-seq:
 *     key.check_key   => true
 *
 *  Raises an exception if the key is invalid.
 *
 *  See the OpenSSL documentation for EC_KEY_check_key()
 */
static VALUE ossl_ec_key_check_key(VALUE self)
{
    EC_KEY *ec;

    Require_EC_KEY(self, ec);

    if (EC_KEY_check_key(ec) != 1)
	ossl_raise(eECError, "EC_KEY_check_key");

    return Qtrue;
}

/*
 *  call-seq:
 *     key.dh_compute_key(pubkey)   => String
 *
 *  See the OpenSSL documentation for ECDH_compute_key()
 */
static VALUE ossl_ec_key_dh_compute_key(VALUE self, VALUE pubkey)
{
    EC_KEY *ec;
    EC_POINT *point;
    int buf_len;
    VALUE str;

    Require_EC_KEY(self, ec);
    SafeRequire_EC_POINT(pubkey, point);

/* BUG: need a way to figure out the maximum string size */
    buf_len = 1024;
    str = rb_str_new(0, buf_len);
/* BUG: take KDF as a block */
    buf_len = ECDH_compute_key(RSTRING_PTR(str), buf_len, point, ec, NULL);
    if (buf_len < 0)
         ossl_raise(eECError, "ECDH_compute_key");

    rb_str_resize(str, buf_len);

    return str;
}

/* sign_setup */

/*
 *  call-seq:
 *     key.dsa_sign_asn1(data)   => String
 *
 *  See the OpenSSL documentation for ECDSA_sign()
 */
static VALUE ossl_ec_key_dsa_sign_asn1(VALUE self, VALUE data)
{
    EC_KEY *ec;
    unsigned int buf_len;
    VALUE str;

    Require_EC_KEY(self, ec);
    StringValue(data);

    if (EC_KEY_get0_private_key(ec) == NULL)
	ossl_raise(eECError, "Private EC key needed!");

    str = rb_str_new(0, ECDSA_size(ec) + 16);
    if (ECDSA_sign(0, (unsigned char *) RSTRING_PTR(data), RSTRING_LENINT(data), (unsigned char *) RSTRING_PTR(str), &buf_len, ec) != 1)
         ossl_raise(eECError, "ECDSA_sign");

    rb_str_resize(str, buf_len);

    return str;
}

/*
 *  call-seq:
 *     key.dsa_verify_asn1(data, sig)   => true or false
 *
 *  See the OpenSSL documentation for ECDSA_verify()
 */
static VALUE ossl_ec_key_dsa_verify_asn1(VALUE self, VALUE data, VALUE sig)
{
    EC_KEY *ec;

    Require_EC_KEY(self, ec);
    StringValue(data);
    StringValue(sig);

    switch (ECDSA_verify(0, (unsigned char *) RSTRING_PTR(data), RSTRING_LENINT(data), (unsigned char *) RSTRING_PTR(sig), (int)RSTRING_LEN(sig), ec)) {
    case 1:	return Qtrue;
    case 0:	return Qfalse;
    default:	break;
    }

    ossl_raise(eECError, "ECDSA_verify");
}

static void ossl_ec_group_free(ossl_ec_group *ec_group)
{
    if (!ec_group->dont_free && ec_group->group)
        EC_GROUP_clear_free(ec_group->group);
    ruby_xfree(ec_group);
}

static VALUE ossl_ec_group_alloc(VALUE klass)
{
    ossl_ec_group *ec_group;
    VALUE obj;

    obj = Data_Make_Struct(klass, ossl_ec_group, 0, ossl_ec_group_free, ec_group);

    return obj;
}

/*  call-seq:
 *     OpenSSL::PKey::EC::Group.new("secp112r1")
 *     OpenSSL::PKey::EC::Group.new(ec_group)
 *     OpenSSL::PKey::EC::Group.new(pem_string)
 *     OpenSSL::PKey::EC::Group.new(der_string)
 *     OpenSSL::PKey::EC::Group.new(pem_file)
 *     OpenSSL::PKey::EC::Group.new(der_file)
 *     OpenSSL::PKey::EC::Group.new(:GFp_simple)
 *     OpenSSL::PKey::EC::Group.new(:GFp_mult)
 *     OpenSSL::PKey::EC::Group.new(:GFp_nist)
 *     OpenSSL::PKey::EC::Group.new(:GF2m_simple)
 *     OpenSSL::PKey::EC::Group.new(:GFp, bignum_p, bignum_a, bignum_b)
 *     OpenSSL::PKey::EC::Group.new(:GF2m, bignum_p, bignum_a, bignum_b)
 *
 *  See the OpenSSL documentation for EC_GROUP_*
 */
static VALUE ossl_ec_group_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE arg1, arg2, arg3, arg4;
    ossl_ec_group *ec_group;
    EC_GROUP *group = NULL;

    Data_Get_Struct(self, ossl_ec_group, ec_group);
    if (ec_group->group != NULL)
        ossl_raise(rb_eRuntimeError, "EC_GROUP is already initialized");

    switch (rb_scan_args(argc, argv, "13", &arg1, &arg2, &arg3, &arg4)) {
    case 1:
        if (SYMBOL_P(arg1)) {
            const EC_METHOD *method = NULL;
            ID id = SYM2ID(arg1);

            if (id == s_GFp_simple) {
                method = EC_GFp_simple_method();
            } else if (id == s_GFp_mont) {
                method = EC_GFp_mont_method();
            } else if (id == s_GFp_nist) {
                method = EC_GFp_nist_method();
            } else if (id == s_GF2m_simple) {
                method = EC_GF2m_simple_method();
            }

            if (method) {
                if ((group = EC_GROUP_new(method)) == NULL)
                    ossl_raise(eEC_GROUP, "EC_GROUP_new");
            } else {
                ossl_raise(rb_eArgError, "unknown symbol, must be :GFp_simple, :GFp_mont, :GFp_nist or :GF2m_simple");
            }
        } else if (rb_obj_is_kind_of(arg1, cEC_GROUP)) {
            const EC_GROUP *arg1_group;

            SafeRequire_EC_GROUP(arg1, arg1_group);
            if ((group = EC_GROUP_dup(arg1_group)) == NULL)
                ossl_raise(eEC_GROUP, "EC_GROUP_dup");
        } else {
            BIO *in = ossl_obj2bio(arg1);

            group = PEM_read_bio_ECPKParameters(in, NULL, NULL, NULL);
            if (!group) {
		OSSL_BIO_reset(in);
                group = d2i_ECPKParameters_bio(in, NULL);
            }

            BIO_free(in);

            if (!group) {
                const char *name = StringValueCStr(arg1);
                int nid = OBJ_sn2nid(name);

		(void)ERR_get_error();
                if (nid == NID_undef)
                    ossl_raise(eEC_GROUP, "unknown curve name (%s)", name);

                group = EC_GROUP_new_by_curve_name(nid);
                if (group == NULL)
                    ossl_raise(eEC_GROUP, "unable to create curve (%s)", name);

                EC_GROUP_set_asn1_flag(group, OPENSSL_EC_NAMED_CURVE);
                EC_GROUP_set_point_conversion_form(group, POINT_CONVERSION_UNCOMPRESSED);
            }
        }

        break;
    case 4:
        if (SYMBOL_P(arg1)) {
            ID id = SYM2ID(arg1);
            EC_GROUP *(*new_curve)(const BIGNUM *, const BIGNUM *, const BIGNUM *, BN_CTX *) = NULL;
            const BIGNUM *p = GetBNPtr(arg2);
            const BIGNUM *a = GetBNPtr(arg3);
            const BIGNUM *b = GetBNPtr(arg4);

            if (id == s_GFp) {
                new_curve = EC_GROUP_new_curve_GFp;
            } else if (id == s_GF2m) {
                new_curve = EC_GROUP_new_curve_GF2m;
            } else {
                ossl_raise(rb_eArgError, "unknown symbol, must be :GFp or :GF2m");
            }

            if ((group = new_curve(p, a, b, ossl_bn_ctx)) == NULL)
                ossl_raise(eEC_GROUP, "EC_GROUP_new_by_GF*");
        } else {
             ossl_raise(rb_eArgError, "unknown argument, must be :GFp or :GF2m");
        }

        break;
    default:
        ossl_raise(rb_eArgError, "wrong number of arguments");
    }

    if (group == NULL)
        ossl_raise(eEC_GROUP, "");

    ec_group->group = group;

    return self;
}

/*  call-seq:
 *     group1 == group2   => true | false
 *
 */
static VALUE ossl_ec_group_eql(VALUE a, VALUE b)
{
    EC_GROUP *group1 = NULL, *group2 = NULL;

    Require_EC_GROUP(a, group1);
    SafeRequire_EC_GROUP(b, group2);

    if (EC_GROUP_cmp(group1, group2, ossl_bn_ctx) == 1)
       return Qfalse;

    return Qtrue;
}

/*  call-seq:
 *     group.generator   => ec_point
 *
 *  See the OpenSSL documentation for EC_GROUP_get0_generator()
 */
static VALUE ossl_ec_group_get_generator(VALUE self)
{
    VALUE point_obj;
    EC_GROUP *group = NULL;

    Require_EC_GROUP(self, group);

    point_obj = ossl_ec_point_dup(EC_GROUP_get0_generator(group), self);

    return point_obj;
}

/*  call-seq:
 *     group.set_generator(generator, order, cofactor)   => self
 *
 *  See the OpenSSL documentation for EC_GROUP_set_generator()
 */
static VALUE ossl_ec_group_set_generator(VALUE self, VALUE generator, VALUE order, VALUE cofactor)
{
    EC_GROUP *group = NULL;
    const EC_POINT *point;
    const BIGNUM *o, *co;

    Require_EC_GROUP(self, group);
    SafeRequire_EC_POINT(generator, point);
    o = GetBNPtr(order);
    co = GetBNPtr(cofactor);

    if (EC_GROUP_set_generator(group, point, o, co) != 1)
        ossl_raise(eEC_GROUP, "EC_GROUP_set_generator");

    return self;
}

/*  call-seq:
 *     group.get_order   => order_bn
 *
 *  See the OpenSSL documentation for EC_GROUP_get_order()
 */
static VALUE ossl_ec_group_get_order(VALUE self)
{
    VALUE bn_obj;
    BIGNUM *bn;
    EC_GROUP *group = NULL;

    Require_EC_GROUP(self, group);

    bn_obj = ossl_bn_new(NULL);
    bn = GetBNPtr(bn_obj);

    if (EC_GROUP_get_order(group, bn, ossl_bn_ctx) != 1)
        ossl_raise(eEC_GROUP, "EC_GROUP_get_order");

    return bn_obj;
}

/*  call-seq:
 *     group.get_cofactor   => cofactor_bn
 *
 *  See the OpenSSL documentation for EC_GROUP_get_cofactor()
 */
static VALUE ossl_ec_group_get_cofactor(VALUE self)
{
    VALUE bn_obj;
    BIGNUM *bn;
    EC_GROUP *group = NULL;

    Require_EC_GROUP(self, group);

    bn_obj = ossl_bn_new(NULL);
    bn = GetBNPtr(bn_obj);

    if (EC_GROUP_get_cofactor(group, bn, ossl_bn_ctx) != 1)
        ossl_raise(eEC_GROUP, "EC_GROUP_get_cofactor");

    return bn_obj;
}

/*  call-seq:
 *     group.curve_name  => String
 *
 *  See the OpenSSL documentation for EC_GROUP_get_curve_name()
 */
static VALUE ossl_ec_group_get_curve_name(VALUE self)
{
    EC_GROUP *group = NULL;
    int nid;

    Get_EC_GROUP(self, group);
    if (group == NULL)
        return Qnil;

    nid = EC_GROUP_get_curve_name(group);

/* BUG: an nid or asn1 object should be returned, maybe. */
    return rb_str_new2(OBJ_nid2sn(nid));
}

/*  call-seq:
 *     EC.builtin_curves => [[name, comment], ...]
 *
 *  See the OpenSSL documentation for EC_builtin_curves()
 */
static VALUE ossl_s_builtin_curves(VALUE self)
{
    EC_builtin_curve *curves = NULL;
    int n;
    int crv_len = rb_long2int(EC_get_builtin_curves(NULL, 0));
    VALUE ary, ret;

    curves = ALLOCA_N(EC_builtin_curve, crv_len);
    if (curves == NULL)
        return Qnil;
    if (!EC_get_builtin_curves(curves, crv_len))
        ossl_raise(rb_eRuntimeError, "EC_get_builtin_curves");

    ret = rb_ary_new2(crv_len);

    for (n = 0; n < crv_len; n++) {
        const char *sname = OBJ_nid2sn(curves[n].nid);
        const char *comment = curves[n].comment;

        ary = rb_ary_new2(2);
        rb_ary_push(ary, rb_str_new2(sname));
        rb_ary_push(ary, comment ? rb_str_new2(comment) : Qnil);
        rb_ary_push(ret, ary);
    }

    return ret;
}

/*  call-seq:
 *     group.asn1_flag  => Fixnum
 *
 *  See the OpenSSL documentation for EC_GROUP_get_asn1_flag()
 */
static VALUE ossl_ec_group_get_asn1_flag(VALUE self)
{
    EC_GROUP *group = NULL;
    int flag;

    Require_EC_GROUP(self, group);

    flag = EC_GROUP_get_asn1_flag(group);

    return INT2FIX(flag);
}

/*  call-seq:
 *     group.asn1_flag = Fixnum   => Fixnum
 *
 *  See the OpenSSL documentation for EC_GROUP_set_asn1_flag()
 */
static VALUE ossl_ec_group_set_asn1_flag(VALUE self, VALUE flag_v)
{
    EC_GROUP *group = NULL;

    Require_EC_GROUP(self, group);

    EC_GROUP_set_asn1_flag(group, NUM2INT(flag_v));

    return flag_v;
}

/*  call-seq:
 *     group.point_conversion_form  => :uncompressed | :compressed | :hybrid
 *
 *  See the OpenSSL documentation for EC_GROUP_get_point_conversion_form()
 */
static VALUE ossl_ec_group_get_point_conversion_form(VALUE self)
{
    EC_GROUP *group = NULL;
    point_conversion_form_t form;
    VALUE ret;

    Require_EC_GROUP(self, group);

    form = EC_GROUP_get_point_conversion_form(group);

    switch (form) {
    case POINT_CONVERSION_UNCOMPRESSED:	ret = ID_uncompressed; break;
    case POINT_CONVERSION_COMPRESSED:	ret = ID_compressed; break;
    case POINT_CONVERSION_HYBRID:	ret = ID_hybrid; break;
    default:	ossl_raise(eEC_GROUP, "unsupported point conversion form: %d, this module should be updated", form);
    }

   return ID2SYM(ret);
}

/*  call-seq:
 *     group.point_conversion_form = form => form
 *
 *  See the OpenSSL documentation for EC_GROUP_set_point_conversion_form()
 */
static VALUE ossl_ec_group_set_point_conversion_form(VALUE self, VALUE form_v)
{
    EC_GROUP *group = NULL;
    point_conversion_form_t form;
    ID form_id = SYM2ID(form_v);

    Require_EC_GROUP(self, group);

    if (form_id == ID_uncompressed) {
        form = POINT_CONVERSION_UNCOMPRESSED;
    } else if (form_id == ID_compressed) {
        form = POINT_CONVERSION_COMPRESSED;
    } else if (form_id == ID_hybrid) {
        form = POINT_CONVERSION_HYBRID;
    } else {
        ossl_raise(rb_eArgError, "form must be :compressed, :uncompressed, or :hybrid");
    }

    EC_GROUP_set_point_conversion_form(group, form);

    return form_v;
}

/*  call-seq:
 *     group.seed   => String or nil
 *
 *  See the OpenSSL documentation for EC_GROUP_get0_seed()
 */
static VALUE ossl_ec_group_get_seed(VALUE self)
{
    EC_GROUP *group = NULL;
    size_t seed_len;

    Require_EC_GROUP(self, group);

    seed_len = EC_GROUP_get_seed_len(group);

    if (seed_len == 0)
        return Qnil;

    return rb_str_new((const char *)EC_GROUP_get0_seed(group), seed_len);
}

/*  call-seq:
 *     group.seed = seed  => seed
 *
 *  See the OpenSSL documentation for EC_GROUP_set_seed()
 */
static VALUE ossl_ec_group_set_seed(VALUE self, VALUE seed)
{
    EC_GROUP *group = NULL;

    Require_EC_GROUP(self, group);
    StringValue(seed);

    if (EC_GROUP_set_seed(group, (unsigned char *)RSTRING_PTR(seed), RSTRING_LEN(seed)) != (size_t)RSTRING_LEN(seed))
        ossl_raise(eEC_GROUP, "EC_GROUP_set_seed");

    return seed;
}

/* get/set curve GFp, GF2m */

/*  call-seq:
 *     group.degree   => Fixnum
 *
 *  See the OpenSSL documentation for EC_GROUP_get_degree()
 */
static VALUE ossl_ec_group_get_degree(VALUE self)
{
    EC_GROUP *group = NULL;

    Require_EC_GROUP(self, group);

    return INT2NUM(EC_GROUP_get_degree(group));
}

static VALUE ossl_ec_group_to_string(VALUE self, int format)
{
    EC_GROUP *group;
    BIO *out;
    int i = -1;
    VALUE str;

    Get_EC_GROUP(self, group);

    if (!(out = BIO_new(BIO_s_mem())))
        ossl_raise(eEC_GROUP, "BIO_new(BIO_s_mem())");

    switch(format) {
    case EXPORT_PEM:
        i = PEM_write_bio_ECPKParameters(out, group);
    	break;
    case EXPORT_DER:
        i = i2d_ECPKParameters_bio(out, group);
    	break;
    default:
        BIO_free(out);
    	ossl_raise(rb_eRuntimeError, "unknown format (internal error)");
    }

    if (i != 1) {
        BIO_free(out);
        ossl_raise(eECError, NULL);
    }

    str = ossl_membio2str(out);

    return str;
}

/*  call-seq:
 *     group.to_pem   => String
 *
 *  See the OpenSSL documentation for PEM_write_bio_ECPKParameters()
 */
static VALUE ossl_ec_group_to_pem(VALUE self)
{
    return ossl_ec_group_to_string(self, EXPORT_PEM);
}

/*  call-seq:
 *     group.to_der   => String
 *
 *  See the OpenSSL documentation for i2d_ECPKParameters_bio()
 */
static VALUE ossl_ec_group_to_der(VALUE self)
{
    return ossl_ec_group_to_string(self, EXPORT_DER);
}

/*  call-seq:
 *     group.to_text   => String
 *
 *  See the OpenSSL documentation for ECPKParameters_print()
 */
static VALUE ossl_ec_group_to_text(VALUE self)
{
    EC_GROUP *group;
    BIO *out;
    VALUE str;

    Require_EC_GROUP(self, group);
    if (!(out = BIO_new(BIO_s_mem()))) {
	ossl_raise(eEC_GROUP, "BIO_new(BIO_s_mem())");
    }
    if (!ECPKParameters_print(out, group, 0)) {
	BIO_free(out);
	ossl_raise(eEC_GROUP, NULL);
    }
    str = ossl_membio2str(out);

    return str;
}


static void ossl_ec_point_free(ossl_ec_point *ec_point)
{
    if (!ec_point->dont_free && ec_point->point)
        EC_POINT_clear_free(ec_point->point);
    ruby_xfree(ec_point);
}

static VALUE ossl_ec_point_alloc(VALUE klass)
{
    ossl_ec_point *ec_point;
    VALUE obj;

    obj = Data_Make_Struct(klass, ossl_ec_point, 0, ossl_ec_point_free, ec_point);

    return obj;
}

/*
 *  call-seq:
 *     OpenSSL::PKey::EC::Point.new(point)
 *     OpenSSL::PKey::EC::Point.new(group)
 *     OpenSSL::PKey::EC::Point.new(group, bn)
 *
 *  See the OpenSSL documentation for EC_POINT_*
 */
static VALUE ossl_ec_point_initialize(int argc, VALUE *argv, VALUE self)
{
    ossl_ec_point *ec_point;
    EC_POINT *point = NULL;
    VALUE arg1, arg2;
    VALUE group_v = Qnil;
    const EC_GROUP *group = NULL;

    Data_Get_Struct(self, ossl_ec_point, ec_point);
    if (ec_point->point)
        ossl_raise(eEC_POINT, "EC_POINT already initialized");

    switch (rb_scan_args(argc, argv, "11", &arg1, &arg2)) {
    case 1:
        if (rb_obj_is_kind_of(arg1, cEC_POINT)) {
            const EC_POINT *arg_point;

            group_v = rb_iv_get(arg1, "@group");
            SafeRequire_EC_GROUP(group_v, group);
            SafeRequire_EC_POINT(arg1, arg_point);

            point = EC_POINT_dup(arg_point, group);
        } else if (rb_obj_is_kind_of(arg1, cEC_GROUP)) {
            group_v = arg1;
            SafeRequire_EC_GROUP(group_v, group);

            point = EC_POINT_new(group);
        } else {
            ossl_raise(eEC_POINT, "wrong argument type: must be OpenSSL::PKey::EC::Point or OpenSSL::Pkey::EC::Group");
        }

        break;
     case 2:
        if (!rb_obj_is_kind_of(arg1, cEC_GROUP))
            ossl_raise(rb_eArgError, "1st argument must be OpenSSL::PKey::EC::Group");
        group_v = arg1;
        SafeRequire_EC_GROUP(group_v, group);

        if (rb_obj_is_kind_of(arg2, cBN)) {
            const BIGNUM *bn = GetBNPtr(arg2);

            point = EC_POINT_bn2point(group, bn, NULL, ossl_bn_ctx);
        } else {
            BIO *in = ossl_obj2bio(arg1);

/* BUG: finish me */

            BIO_free(in);

            if (point == NULL) {
                ossl_raise(eEC_POINT, "unknown type for 2nd arg");
            }
        }
        break;
    default:
        ossl_raise(rb_eArgError, "wrong number of arguments");
    }

    if (point == NULL)
        ossl_raise(eEC_POINT, NULL);

    if (NIL_P(group_v))
        ossl_raise(rb_eRuntimeError, "missing group (internal error)");

    ec_point->point = point;

    rb_iv_set(self, "@group", group_v);

    return self;
}

/*
 *  call-seq:
 *     point1 == point2 => true | false
 *
 */
static VALUE ossl_ec_point_eql(VALUE a, VALUE b)
{
    EC_POINT *point1, *point2;
    VALUE group_v1 = rb_iv_get(a, "@group");
    VALUE group_v2 = rb_iv_get(b, "@group");
    const EC_GROUP *group;

    if (ossl_ec_group_eql(group_v1, group_v2) == Qfalse)
        return Qfalse;

    Require_EC_POINT(a, point1);
    SafeRequire_EC_POINT(b, point2);
    SafeRequire_EC_GROUP(group_v1, group);

    if (EC_POINT_cmp(group, point1, point2, ossl_bn_ctx) == 1)
        return Qfalse;

    return Qtrue;
}

/*
 *  call-seq:
 *     point.infinity? => true | false
 *
 */
static VALUE ossl_ec_point_is_at_infinity(VALUE self)
{
    EC_POINT *point;
    VALUE group_v = rb_iv_get(self, "@group");
    const EC_GROUP *group;

    Require_EC_POINT(self, point);
    SafeRequire_EC_GROUP(group_v, group);

    switch (EC_POINT_is_at_infinity(group, point)) {
    case 1: return Qtrue;
    case 0: return Qfalse;
    default: ossl_raise(cEC_POINT, "EC_POINT_is_at_infinity");
    }
}

/*
 *  call-seq:
 *     point.on_curve? => true | false
 *
 */
static VALUE ossl_ec_point_is_on_curve(VALUE self)
{
    EC_POINT *point;
    VALUE group_v = rb_iv_get(self, "@group");
    const EC_GROUP *group;

    Require_EC_POINT(self, point);
    SafeRequire_EC_GROUP(group_v, group);

    switch (EC_POINT_is_on_curve(group, point, ossl_bn_ctx)) {
    case 1: return Qtrue;
    case 0: return Qfalse;
    default: ossl_raise(cEC_POINT, "EC_POINT_is_on_curve");
    }
}

/*
 *  call-seq:
 *     point.make_affine! => self
 *
 */
static VALUE ossl_ec_point_make_affine(VALUE self)
{
    EC_POINT *point;
    VALUE group_v = rb_iv_get(self, "@group");
    const EC_GROUP *group;

    Require_EC_POINT(self, point);
    SafeRequire_EC_GROUP(group_v, group);

    if (EC_POINT_make_affine(group, point, ossl_bn_ctx) != 1)
        ossl_raise(cEC_POINT, "EC_POINT_make_affine");

    return self;
}

/*
 *  call-seq:
 *     point.invert! => self
 *
 */
static VALUE ossl_ec_point_invert(VALUE self)
{
    EC_POINT *point;
    VALUE group_v = rb_iv_get(self, "@group");
    const EC_GROUP *group;

    Require_EC_POINT(self, point);
    SafeRequire_EC_GROUP(group_v, group);

    if (EC_POINT_invert(group, point, ossl_bn_ctx) != 1)
        ossl_raise(cEC_POINT, "EC_POINT_invert");

    return self;
}

/*
 *  call-seq:
 *     point.set_to_infinity! => self
 *
 */
static VALUE ossl_ec_point_set_to_infinity(VALUE self)
{
    EC_POINT *point;
    VALUE group_v = rb_iv_get(self, "@group");
    const EC_GROUP *group;

    Require_EC_POINT(self, point);
    SafeRequire_EC_GROUP(group_v, group);

    if (EC_POINT_set_to_infinity(group, point) != 1)
        ossl_raise(cEC_POINT, "EC_POINT_set_to_infinity");

    return self;
}

/*
 *  call-seq:
 *     point.to_bn   => OpenSSL::BN
 *
 *  See the OpenSSL documentation for EC_POINT_point2bn()
 */
static VALUE ossl_ec_point_to_bn(VALUE self)
{
    EC_POINT *point;
    VALUE bn_obj;
    VALUE group_v = rb_iv_get(self, "@group");
    const EC_GROUP *group;
    point_conversion_form_t form;
    BIGNUM *bn;

    Require_EC_POINT(self, point);
    SafeRequire_EC_GROUP(group_v, group);

    form = EC_GROUP_get_point_conversion_form(group);

    bn_obj = rb_obj_alloc(cBN);
    bn = GetBNPtr(bn_obj);

    if (EC_POINT_point2bn(group, point, form, bn, ossl_bn_ctx) == NULL)
        ossl_raise(eEC_POINT, "EC_POINT_point2bn");

    return bn_obj;
}

static void no_copy(VALUE klass)
{
    rb_undef_method(klass, "copy");
    rb_undef_method(klass, "clone");
    rb_undef_method(klass, "dup");
    rb_undef_method(klass, "initialize_copy");
}

void Init_ossl_ec()
{
#ifdef DONT_NEED_RDOC_WORKAROUND
    mOSSL = rb_define_module("OpenSSL");
    mPKey = rb_define_module_under(mOSSL, "PKey");
#endif

    eECError = rb_define_class_under(mPKey, "ECError", ePKeyError);

    cEC = rb_define_class_under(mPKey, "EC", cPKey);
    cEC_GROUP = rb_define_class_under(cEC, "Group", rb_cObject);
    cEC_POINT = rb_define_class_under(cEC, "Point", rb_cObject);
    eEC_GROUP = rb_define_class_under(cEC_GROUP, "Error", eOSSLError);
    eEC_POINT = rb_define_class_under(cEC_POINT, "Error", eOSSLError);

    s_GFp = rb_intern("GFp");
    s_GF2m = rb_intern("GF2m");
    s_GFp_simple = rb_intern("GFp_simple");
    s_GFp_mont = rb_intern("GFp_mont");
    s_GFp_nist = rb_intern("GFp_nist");
    s_GF2m_simple = rb_intern("GF2m_simple");

    ID_uncompressed = rb_intern("uncompressed");
    ID_compressed = rb_intern("compressed");
    ID_hybrid = rb_intern("hybrid");

#ifdef OPENSSL_EC_NAMED_CURVE
    rb_define_const(cEC, "NAMED_CURVE", ULONG2NUM(OPENSSL_EC_NAMED_CURVE));
#endif

    rb_define_singleton_method(cEC, "builtin_curves", ossl_s_builtin_curves, 0);

    rb_define_method(cEC, "initialize", ossl_ec_key_initialize, -1);
/* copy/dup/cmp */

    rb_define_method(cEC, "group", ossl_ec_key_get_group, 0);
    rb_define_method(cEC, "group=", ossl_ec_key_set_group, 1);
    rb_define_method(cEC, "private_key", ossl_ec_key_get_private_key, 0);
    rb_define_method(cEC, "private_key=", ossl_ec_key_set_private_key, 1);
    rb_define_method(cEC, "public_key", ossl_ec_key_get_public_key, 0);
    rb_define_method(cEC, "public_key=", ossl_ec_key_set_public_key, 1);
    rb_define_method(cEC, "private_key?", ossl_ec_key_is_private_key, 0);
    rb_define_method(cEC, "public_key?", ossl_ec_key_is_public_key, 0);
/*  rb_define_method(cEC, "", ossl_ec_key_get_, 0);
    rb_define_method(cEC, "=", ossl_ec_key_set_ 1);
    set/get enc_flags
    set/get _conv_from
    set/get asn1_flag (can use ruby to call self.group.asn1_flag)
    set/get precompute_mult
*/
    rb_define_method(cEC, "generate_key", ossl_ec_key_generate_key, 0);
    rb_define_method(cEC, "check_key", ossl_ec_key_check_key, 0);

    rb_define_method(cEC, "dh_compute_key", ossl_ec_key_dh_compute_key, 1);
    rb_define_method(cEC, "dsa_sign_asn1", ossl_ec_key_dsa_sign_asn1, 1);
    rb_define_method(cEC, "dsa_verify_asn1", ossl_ec_key_dsa_verify_asn1, 2);
/* do_sign/do_verify */

    rb_define_method(cEC, "to_pem", ossl_ec_key_to_pem, -1);
    rb_define_method(cEC, "to_der", ossl_ec_key_to_der, 0);
    rb_define_method(cEC, "to_text", ossl_ec_key_to_text, 0);


    rb_define_alloc_func(cEC_GROUP, ossl_ec_group_alloc);
    rb_define_method(cEC_GROUP, "initialize", ossl_ec_group_initialize, -1);
    rb_define_method(cEC_GROUP, "eql?", ossl_ec_group_eql, 1);
    rb_define_alias(cEC_GROUP, "==", "eql?");
/* copy/dup/cmp */

    rb_define_method(cEC_GROUP, "generator", ossl_ec_group_get_generator, 0);
    rb_define_method(cEC_GROUP, "set_generator", ossl_ec_group_set_generator, 3);
    rb_define_method(cEC_GROUP, "order", ossl_ec_group_get_order, 0);
    rb_define_method(cEC_GROUP, "cofactor", ossl_ec_group_get_cofactor, 0);

    rb_define_method(cEC_GROUP, "curve_name", ossl_ec_group_get_curve_name, 0);
/*    rb_define_method(cEC_GROUP, "curve_name=", ossl_ec_group_set_curve_name, 1); */

    rb_define_method(cEC_GROUP, "asn1_flag", ossl_ec_group_get_asn1_flag, 0);
    rb_define_method(cEC_GROUP, "asn1_flag=", ossl_ec_group_set_asn1_flag, 1);

    rb_define_method(cEC_GROUP, "point_conversion_form", ossl_ec_group_get_point_conversion_form, 0);
    rb_define_method(cEC_GROUP, "point_conversion_form=", ossl_ec_group_set_point_conversion_form, 1);

    rb_define_method(cEC_GROUP, "seed", ossl_ec_group_get_seed, 0);
    rb_define_method(cEC_GROUP, "seed=", ossl_ec_group_set_seed, 1);

/* get/set GFp, GF2m */

    rb_define_method(cEC_GROUP, "degree", ossl_ec_group_get_degree, 0);

/* check* */


    rb_define_method(cEC_GROUP, "to_pem", ossl_ec_group_to_pem, 0);
    rb_define_method(cEC_GROUP, "to_der", ossl_ec_group_to_der, 0);
    rb_define_method(cEC_GROUP, "to_text", ossl_ec_group_to_text, 0);


    rb_define_alloc_func(cEC_POINT, ossl_ec_point_alloc);
    rb_define_method(cEC_POINT, "initialize", ossl_ec_point_initialize, -1);
    rb_attr(cEC_POINT, rb_intern("group"), 1, 0, 0);
    rb_define_method(cEC_POINT, "eql?", ossl_ec_point_eql, 1);
    rb_define_alias(cEC_POINT, "==", "eql?");

    rb_define_method(cEC_POINT, "infinity?", ossl_ec_point_is_at_infinity, 0);
    rb_define_method(cEC_POINT, "on_curve?", ossl_ec_point_is_on_curve, 0);
    rb_define_method(cEC_POINT, "make_affine!", ossl_ec_point_make_affine, 0);
    rb_define_method(cEC_POINT, "invert!", ossl_ec_point_invert, 0);
    rb_define_method(cEC_POINT, "set_to_infinity!", ossl_ec_point_set_to_infinity, 0);
/* all the other methods */

    rb_define_method(cEC_POINT, "to_bn", ossl_ec_point_to_bn, 0);

    no_copy(cEC);
    no_copy(cEC_GROUP);
    no_copy(cEC_POINT);
}

#else /* defined NO_EC */
void Init_ossl_ec()
{
}
#endif /* NO_EC */

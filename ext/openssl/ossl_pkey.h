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
#if !defined(_OSSL_PKEY_H_)
#define _OSSL_PKEY_H_

extern VALUE mPKey;
extern VALUE cPKey;
extern VALUE ePKeyError;
extern ID id_private_q;

#define OSSL_PKEY_SET_PRIVATE(obj) rb_iv_set((obj), "private", Qtrue)
#define OSSL_PKEY_SET_PUBLIC(obj)  rb_iv_set((obj), "private", Qfalse)
#define OSSL_PKEY_IS_PRIVATE(obj)  (rb_iv_get((obj), "private") == Qtrue)

#define WrapPKey(klass, obj, pkey) do { \
    if (!(pkey)) { \
	rb_raise(rb_eRuntimeError, "PKEY wasn't initialized!"); \
    } \
    (obj) = Data_Wrap_Struct((klass), 0, EVP_PKEY_free, (pkey)); \
    OSSL_PKEY_SET_PUBLIC(obj); \
} while (0)
#define GetPKey(obj, pkey) do {\
    Data_Get_Struct((obj), EVP_PKEY, (pkey));\
    if (!(pkey)) { \
	rb_raise(rb_eRuntimeError, "PKEY wasn't initialized!");\
    } \
} while (0)
#define SafeGetPKey(obj, pkey) do { \
    OSSL_Check_Kind((obj), cPKey); \
    GetPKey((obj), (pkey)); \
} while (0)

void ossl_generate_cb(int, int, void *);
#define HAVE_BN_GENCB defined(HAVE_RSA_GENERATE_KEY_EX) || defined(HAVE_DH_GENERATE_PARAMETERS_EX) || defined(HAVE_DSA_GENERATE_PARAMETERS_EX)
#if HAVE_BN_GENCB
struct ossl_generate_cb_arg {
    int yield;
    int stop;
    int state;
};
int ossl_generate_cb_2(int p, int n, BN_GENCB *cb);
void ossl_generate_cb_stop(void *ptr);
#endif

VALUE ossl_pkey_new(EVP_PKEY *);
VALUE ossl_pkey_new_from_file(VALUE);
EVP_PKEY *GetPKeyPtr(VALUE);
EVP_PKEY *DupPKeyPtr(VALUE);
EVP_PKEY *GetPrivPKeyPtr(VALUE);
EVP_PKEY *DupPrivPKeyPtr(VALUE);
void Init_ossl_pkey(void);

/*
 * RSA
 */
extern VALUE cRSA;
extern VALUE eRSAError;

VALUE ossl_rsa_new(EVP_PKEY *);
void Init_ossl_rsa(void);

/*
 * DSA
 */
extern VALUE cDSA;
extern VALUE eDSAError;

VALUE ossl_dsa_new(EVP_PKEY *);
void Init_ossl_dsa(void);

/*
 * DH
 */
extern VALUE cDH;
extern VALUE eDHError;
extern DH *OSSL_DEFAULT_DH_512;
extern DH *OSSL_DEFAULT_DH_1024;

VALUE ossl_dh_new(EVP_PKEY *);
void Init_ossl_dh(void);

/*
 * EC
 */
extern VALUE cEC;
extern VALUE eECError;
extern VALUE cEC_GROUP;
extern VALUE eEC_GROUP;
extern VALUE cEC_POINT;
extern VALUE eEC_POINT;
VALUE ossl_ec_new(EVP_PKEY *);
void Init_ossl_ec(void);


#define OSSL_PKEY_BN(keytype, name)					\
/*									\
 *  call-seq:								\
 *     key.##name -> aBN						\
 */									\
static VALUE ossl_##keytype##_get_##name(VALUE self)			\
{									\
	EVP_PKEY *pkey;							\
	BIGNUM *bn;							\
									\
	GetPKey(self, pkey);						\
	bn = pkey->pkey.keytype->name;					\
	if (bn == NULL)							\
		return Qnil;						\
	return ossl_bn_new(bn);						\
}									\
/*									\
 *  call-seq:								\
 *     key.##name = bn -> bn						\
 */									\
static VALUE ossl_##keytype##_set_##name(VALUE self, VALUE bignum)	\
{									\
	EVP_PKEY *pkey;							\
	BIGNUM *bn;							\
									\
	GetPKey(self, pkey);						\
	if (NIL_P(bignum)) {						\
		BN_clear_free(pkey->pkey.keytype->name);		\
		pkey->pkey.keytype->name = NULL;			\
		return Qnil;						\
	}								\
									\
	bn = GetBNPtr(bignum);						\
	if (pkey->pkey.keytype->name == NULL)				\
		pkey->pkey.keytype->name = BN_new();			\
	if (pkey->pkey.keytype->name == NULL)				\
		ossl_raise(eBNError, NULL);				\
	if (BN_copy(pkey->pkey.keytype->name, bn) == NULL)		\
		ossl_raise(eBNError, NULL);				\
	return bignum;							\
}

#define DEF_OSSL_PKEY_BN(class, keytype, name)				\
do {									\
	rb_define_method((class), #name, ossl_##keytype##_get_##name, 0);	\
	rb_define_method((class), #name "=", ossl_##keytype##_set_##name, 1);\
} while (0)

#endif /* _OSSL_PKEY_H_ */

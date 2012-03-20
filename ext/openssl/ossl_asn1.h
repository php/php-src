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
#if !defined(_OSSL_ASN1_H_)
#define _OSSL_ASN1_H_

/*
 * ASN1_DATE conversions
 */
VALUE asn1time_to_time(ASN1_TIME *);
time_t time_to_time_t(VALUE);

/*
 * ASN1_STRING conversions
 */
VALUE asn1str_to_str(ASN1_STRING *);

/*
 * ASN1_INTEGER conversions
 */
VALUE asn1integer_to_num(ASN1_INTEGER *);
ASN1_INTEGER *num_to_asn1integer(VALUE, ASN1_INTEGER *);

/*
 * ASN1 module
 */
extern VALUE mASN1;
extern VALUE eASN1Error;

extern VALUE cASN1Data;
extern VALUE cASN1Primitive;
extern VALUE cASN1Constructive;

extern VALUE cASN1Boolean;                           /* BOOLEAN           */
extern VALUE cASN1Integer, cASN1Enumerated;          /* INTEGER           */
extern VALUE cASN1BitString;                         /* BIT STRING        */
extern VALUE cASN1OctetString, cASN1UTF8String;      /* STRINGs           */
extern VALUE cASN1NumericString, cASN1PrintableString;
extern VALUE cASN1T61String, cASN1VideotexString;
extern VALUE cASN1IA5String, cASN1GraphicString;
extern VALUE cASN1ISO64String, cASN1GeneralString;
extern VALUE cASN1UniversalString, cASN1BMPString;
extern VALUE cASN1Null;                              /* NULL              */
extern VALUE cASN1ObjectId;                          /* OBJECT IDENTIFIER */
extern VALUE cASN1UTCTime, cASN1GeneralizedTime;     /* TIME              */
extern VALUE cASN1Sequence, cASN1Set;                /* CONSTRUCTIVE      */

ASN1_TYPE *ossl_asn1_get_asn1type(VALUE);

void Init_ossl_asn1(void);

#endif

/* This is a generated file, edit encoding.stub.php instead.
 * Stub hash: ada1d3a8882e099da6ed20ce66a7c6ebacf95dee */

#ifndef ZEND_ENCODING_DECL_ada1d3a8882e099da6ed20ce66a7c6ebacf95dee_H
#define ZEND_ENCODING_DECL_ada1d3a8882e099da6ed20ce66a7c6ebacf95dee_H

typedef enum zend_enum_Encoding_Base16 {
	ZEND_ENUM_Encoding_Base16_Upper = 1,
	ZEND_ENUM_Encoding_Base16_Lower = 2,
} zend_enum_Encoding_Base16;

typedef enum zend_enum_Encoding_Base32 {
	ZEND_ENUM_Encoding_Base32_Ascii = 1,
	ZEND_ENUM_Encoding_Base32_Hex = 2,
	ZEND_ENUM_Encoding_Base32_Crockford = 3,
	ZEND_ENUM_Encoding_Base32_Z = 4,
} zend_enum_Encoding_Base32;

typedef enum zend_enum_Encoding_Base58 {
	ZEND_ENUM_Encoding_Base58_Bitcoin = 1,
	ZEND_ENUM_Encoding_Base58_Flickr = 2,
} zend_enum_Encoding_Base58;

typedef enum zend_enum_Encoding_Base64 {
	ZEND_ENUM_Encoding_Base64_Standard = 1,
	ZEND_ENUM_Encoding_Base64_UrlSafe = 2,
	ZEND_ENUM_Encoding_Base64_Imap = 3,
} zend_enum_Encoding_Base64;

typedef enum zend_enum_Encoding_Base85 {
	ZEND_ENUM_Encoding_Base85_Adobe = 1,
	ZEND_ENUM_Encoding_Base85_Z85 = 2,
	ZEND_ENUM_Encoding_Base85_Git = 3,
} zend_enum_Encoding_Base85;

typedef enum zend_enum_Encoding_PaddingMode {
	ZEND_ENUM_Encoding_PaddingMode_VariantControlled = 1,
	ZEND_ENUM_Encoding_PaddingMode_StripPadding = 2,
	ZEND_ENUM_Encoding_PaddingMode_PreservePadding = 3,
} zend_enum_Encoding_PaddingMode;

typedef enum zend_enum_Encoding_DecodingMode {
	ZEND_ENUM_Encoding_DecodingMode_Forgiving = 1,
	ZEND_ENUM_Encoding_DecodingMode_Strict = 2,
} zend_enum_Encoding_DecodingMode;

typedef enum zend_enum_Encoding_TimingMode {
	ZEND_ENUM_Encoding_TimingMode_Variable = 1,
	ZEND_ENUM_Encoding_TimingMode_Constant = 2,
} zend_enum_Encoding_TimingMode;

#endif /* ZEND_ENCODING_DECL_ada1d3a8882e099da6ed20ce66a7c6ebacf95dee_H */

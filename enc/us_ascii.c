#include "regenc.h"

static int
us_ascii_mbc_enc_len(const UChar* p, const UChar* e, OnigEncoding enc)
{
    if (*p & 0x80)
        return ONIGENC_CONSTRUCT_MBCLEN_INVALID();
    return ONIGENC_CONSTRUCT_MBCLEN_CHARFOUND(1);
}

OnigEncodingDefine(us_ascii, US_ASCII) = {
  us_ascii_mbc_enc_len,
  "US-ASCII",/* name */
  1,           /* max byte length */
  1,           /* min byte length */
  onigenc_is_mbc_newline_0x0a,
  onigenc_single_byte_mbc_to_code,
  onigenc_single_byte_code_to_mbclen,
  onigenc_single_byte_code_to_mbc,
  onigenc_ascii_mbc_case_fold,
  onigenc_ascii_apply_all_case_fold,
  onigenc_ascii_get_case_fold_codes_by_str,
  onigenc_minimum_property_name_to_ctype,
  onigenc_ascii_is_code_ctype,
  onigenc_not_support_get_ctype_code_range,
  onigenc_single_byte_left_adjust_char_head,
  onigenc_always_true_is_allowed_reverse_match,
  0,
  ONIGENC_FLAG_NONE,
};
ENC_ALIAS("ASCII", "US-ASCII")
ENC_ALIAS("ANSI_X3.4-1968", "US-ASCII")
ENC_ALIAS("646", "US-ASCII")

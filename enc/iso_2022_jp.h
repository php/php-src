#include "regenc.h"
/* dummy for unsupported, statefull encoding */
ENC_DUMMY("ISO-2022-JP");
ENC_ALIAS("ISO2022-JP", "ISO-2022-JP");
ENC_REPLICATE("ISO-2022-JP-2", "ISO-2022-JP");
ENC_ALIAS("ISO2022-JP2", "ISO-2022-JP-2");

/*
 * Name: CP50220
 * MIBenum: 2260
 * Link: http://www.iana.org/assignments/charset-reg/CP50220
 *
 * Windows Codepage 50220
 * a ISO-2022-JP variant.
 * This includes
 * * US-ASCII
 * * JIS X 0201 Latin
 * * JIS X 0201 Katakana
 * * JIS X 0208
 * * NEC special characters
 * * NEC selected IBM extended characters
 * and this implementation doesn't include
 * * User Defined Characters
 *
 * So this CP50220 has the same characters of CP51932.
 *
 * See http://legacy-encoding.sourceforge.jp/wiki/index.php?cp50220
 */
ENC_REPLICATE("CP50220", "ISO-2022-JP");

/* Windows Codepage 50221
 * a ISO-2022-JP variant.
 * This includes
 * * US-ASCII
 * * JIS X 0201 Latin
 * * JIS X 0201 Katakana
 * * JIS X 0208
 * * NEC special characters
 * * NEC selected IBM extended characters
 * and this implementation doesn't include
 * * User Defined Characters
 *
 * So this CP50221 has the same characters of CP51932.
 *
 * See http://legacy-encoding.sourceforge.jp/wiki/index.php?cp50221
 */
ENC_REPLICATE("CP50221", "ISO-2022-JP");

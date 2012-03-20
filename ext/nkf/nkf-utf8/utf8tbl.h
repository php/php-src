/*
 * utf8tbl.h - Header file for Convertion Table
 *
 * $Id$
 */

#ifndef _UTF8TBL_H_
#define _UTF8TBL_H_

#ifdef UTF8_OUTPUT_ENABLE
#define sizeof_euc_to_utf8_1byte 94
#define sizeof_euc_to_utf8_2bytes 94
extern const unsigned short euc_to_utf8_1byte[];
extern const unsigned short *const euc_to_utf8_2bytes[];
extern const unsigned short *const euc_to_utf8_2bytes_ms[];
extern const unsigned short *const euc_to_utf8_2bytes_mac[];
extern const unsigned short *const x0212_to_utf8_2bytes[];
#endif /* UTF8_OUTPUT_ENABLE */

#ifdef UTF8_INPUT_ENABLE
#define sizeof_utf8_to_euc_C2 64
#define sizeof_utf8_to_euc_E5B8 64
#define sizeof_utf8_to_euc_2bytes 112
#define sizeof_utf8_to_euc_3bytes 16
extern const unsigned short *const utf8_to_euc_2bytes[];
extern const unsigned short *const utf8_to_euc_2bytes_ms[];
extern const unsigned short *const utf8_to_euc_2bytes_932[];
extern const unsigned short *const utf8_to_euc_2bytes_mac[];
extern const unsigned short *const *const utf8_to_euc_3bytes[];
extern const unsigned short *const *const utf8_to_euc_3bytes_ms[];
extern const unsigned short *const *const utf8_to_euc_3bytes_932[];
extern const unsigned short *const *const utf8_to_euc_3bytes_mac[];
#endif /* UTF8_INPUT_ENABLE */

#ifdef UNICODE_NORMALIZATION

#define NORMALIZATION_TABLE_LENGTH 942
#define NORMALIZATION_TABLE_NFC_LENGTH 3
#define NORMALIZATION_TABLE_NFD_LENGTH 9
struct normalization_pair {
    const unsigned char nfc[NORMALIZATION_TABLE_NFC_LENGTH];
    const unsigned char nfd[NORMALIZATION_TABLE_NFD_LENGTH];
};
extern const struct normalization_pair normalization_table[];
#endif

#ifdef SHIFTJIS_CP932
#define CP932_TABLE_BEGIN 0xFA
#define CP932_TABLE_END   0xFC
extern const unsigned short shiftjis_cp932[3][189];
#define CP932INV_TABLE_BEGIN 0xED
#define CP932INV_TABLE_END   0xEE
extern const unsigned short cp932inv[2][189];
#endif /* SHIFTJIS_CP932 */

#ifdef X0212_ENABLE
extern const unsigned short shiftjis_x0212[3][189];
extern const unsigned short *const x0212_shiftjis[];
#endif /* X0212_ENABLE */

#endif

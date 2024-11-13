/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SYNTAX_STATE_RES_H
#define LEXBOR_CSS_SYNTAX_STATE_RES_H


static const lxb_css_syntax_tokenizer_state_f
lxb_css_syntax_state_res_map[256] =
{
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x00; 'NUL'; NULL */
    lxb_css_syntax_state_delim, /* 0x01; 'SOH'; Start of Heading */
    lxb_css_syntax_state_delim, /* 0x02; 'STX'; Start of text */
    lxb_css_syntax_state_delim, /* 0x03; 'ETX'; End of text */
    lxb_css_syntax_state_delim, /* 0x04; 'EOT'; End of Transmission */
    lxb_css_syntax_state_delim, /* 0x05; 'ENQ'; Enquiry */
    lxb_css_syntax_state_delim, /* 0x06; 'ACK'; Acknowledge */
    lxb_css_syntax_state_delim, /* 0x07; 'BEL'; Bell */
    lxb_css_syntax_state_delim, /* 0x08; 'BS'; Backspace */
    lxb_css_syntax_state_whitespace, /* 0x09; 'TAB'; Horizontal Tab */
    lxb_css_syntax_state_whitespace, /* 0x0A; 'LF'; Line Feed ('\n') */
    lxb_css_syntax_state_delim, /* 0x0B; 'VT'; Vertical Tab */
    lxb_css_syntax_state_whitespace, /* 0x0C; 'FF'; Form Feed */
    lxb_css_syntax_state_whitespace, /* 0x0D; 'CR'; Carriage Return ('\r') */
    lxb_css_syntax_state_delim, /* 0x0E; 'SO'; Shift Out */
    lxb_css_syntax_state_delim, /* 0x0F; 'SI'; Shift In */
    lxb_css_syntax_state_delim, /* 0x10; 'DLE'; Data Link Escape */
    lxb_css_syntax_state_delim, /* 0x11; 'DC1'; Device Control #1 */
    lxb_css_syntax_state_delim, /* 0x12; 'DC2'; Device Control #2 */
    lxb_css_syntax_state_delim, /* 0x13; 'DC3'; Device Control #3 */
    lxb_css_syntax_state_delim, /* 0x14; 'DC4'; Device Control #4 */
    lxb_css_syntax_state_delim, /* 0x15; 'NAK'; Negative Acknowledge */
    lxb_css_syntax_state_delim, /* 0x16; 'SYN'; Synchronous Idle */
    lxb_css_syntax_state_delim, /* 0x17; 'ETB'; End of Transmission Block */
    lxb_css_syntax_state_delim, /* 0x18; 'CAN'; Cancel */
    lxb_css_syntax_state_delim, /* 0x19; 'EM'; End of Medium */
    lxb_css_syntax_state_delim, /* 0x1A; 'SUB'; Substitute */
    lxb_css_syntax_state_delim, /* 0x1B; 'ESC'; Escape */
    lxb_css_syntax_state_delim, /* 0x1C; 'FS'; File Separator */
    lxb_css_syntax_state_delim, /* 0x1D; 'GS'; Group Separator */
    lxb_css_syntax_state_delim, /* 0x1E; 'RS'; Record Separator */
    lxb_css_syntax_state_delim, /* 0x1F; 'US'; Unit Separator */
    lxb_css_syntax_state_whitespace, /* 0x20; 'SP'; Space */
    lxb_css_syntax_state_delim, /* 0x21; '!'; Exclamation mark */
    lxb_css_syntax_state_string, /* 0x22; '"'; Only quotes above */
    lxb_css_syntax_state_hash, /* 0x23; '#'; Pound sign */
    lxb_css_syntax_state_delim, /* 0x24; '$'; Dollar sign */
    lxb_css_syntax_state_delim, /* 0x25; '%'; Percentage sign */
    lxb_css_syntax_state_delim, /* 0x26; '&'; Commericial and */
    lxb_css_syntax_state_string, /* 0x27; '''; Apostrophe */
    lxb_css_syntax_state_lparenthesis, /* 0x28; '('; Left bracket */
    lxb_css_syntax_state_rparenthesis, /* 0x29; ')'; Right bracket */
    lxb_css_syntax_state_delim, /* 0x2A; '*'; Asterisk */
    lxb_css_syntax_state_plus, /* 0x2B; '+'; Plus symbol */
    lxb_css_syntax_state_comma, /* 0x2C; ','; Comma */
    lxb_css_syntax_state_minus, /* 0x2D; '-'; Dash */
    lxb_css_syntax_state_full_stop, /* 0x2E; '.'; Full stop */
    lxb_css_syntax_state_comment, /* 0x2F; '/'; Forward slash */
    lxb_css_syntax_state_consume_before_numeric, /* 0x30; '0' */
    lxb_css_syntax_state_consume_before_numeric, /* 0x31; '1' */
    lxb_css_syntax_state_consume_before_numeric, /* 0x32; '2' */
    lxb_css_syntax_state_consume_before_numeric, /* 0x33; '3' */
    lxb_css_syntax_state_consume_before_numeric, /* 0x34; '4' */
    lxb_css_syntax_state_consume_before_numeric, /* 0x35; '5' */
    lxb_css_syntax_state_consume_before_numeric, /* 0x36; '6' */
    lxb_css_syntax_state_consume_before_numeric, /* 0x37; '7' */
    lxb_css_syntax_state_consume_before_numeric, /* 0x38; '8' */
    lxb_css_syntax_state_consume_before_numeric, /* 0x39; '9' */
    lxb_css_syntax_state_colon, /* 0x3A; ':'; Colon */
    lxb_css_syntax_state_semicolon, /* 0x3B; ';'; Semicolon */
    lxb_css_syntax_state_less_sign, /* 0x3C; '<'; Small than bracket */
    lxb_css_syntax_state_delim, /* 0x3D; '='; Equals sign */
    lxb_css_syntax_state_delim, /* 0x3E; '>'; Bigger than symbol */
    lxb_css_syntax_state_delim, /* 0x3F; '?'; Question mark */
    lxb_css_syntax_state_at, /* 0x40; '@'; At symbol */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x41; 'A' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x42; 'B' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x43; 'C' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x44; 'D' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x45; 'E' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x46; 'F' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x47; 'G' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x48; 'H' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x49; 'I' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x4A; 'J' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x4B; 'K' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x4C; 'L' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x4D; 'M' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x4E; 'N' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x4F; 'O' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x50; 'P' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x51; 'Q' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x52; 'R' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x53; 'S' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x54; 'T' */
    lxb_css_syntax_state_ident_like_begin, /* 0x55; 'U' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x56; 'V' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x57; 'W' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x58; 'X' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x59; 'Y' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x5A; 'Z' */
    lxb_css_syntax_state_ls_bracket, /* 0x5B; '['; Left square bracket */
    lxb_css_syntax_state_rsolidus, /* 0x5C; '\'; Inverse/backward slash */
    lxb_css_syntax_state_rs_bracket, /* 0x5D; ']'; Right square bracket */
    lxb_css_syntax_state_delim, /* 0x5E; '^'; Circumflex */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x5F; '_'; Underscore */
    lxb_css_syntax_state_delim, /* 0x60; '`'; Gravis (backtick) */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x61; 'a' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x62; 'b' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x63; 'c' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x64; 'd' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x65; 'e' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x66; 'f' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x67; 'g' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x68; 'h' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x69; 'i' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x6A; 'j' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x6B; 'k' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x6C; 'l' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x6D; 'm' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x6E; 'n' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x6F; 'o' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x70; 'p' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x71; 'q' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x72; 'r' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x73; 's' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x74; 't' */
    lxb_css_syntax_state_ident_like_begin, /* 0x75; 'u' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x76; 'v' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x77; 'w' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x78; 'x' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x79; 'y' */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x7A; 'z' */
    lxb_css_syntax_state_lc_bracket, /* 0x7B; '{'; Left curly bracket */
    lxb_css_syntax_state_delim, /* 0x7C; '|'; Vertical line */
    lxb_css_syntax_state_rc_bracket, /* 0x7D; '}'; Right curly brackets */
    lxb_css_syntax_state_delim, /* 0x7E; '~'; Tilde */
    lxb_css_syntax_state_delim, /* 0x7F; 'DEL'; Delete */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x80 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x81 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x82 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x83 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x84 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x85 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x86 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x87 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x88 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x89 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x8A */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x8B */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x8C */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x8D */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x8E */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x8F */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x90 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x91 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x92 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x93 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x94 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x95 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x96 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x97 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x98 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x99 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x9A */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x9B */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x9C */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x9D */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x9E */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0x9F */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xA0 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xA1 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xA2 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xA3 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xA4 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xA5 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xA6 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xA7 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xA8 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xA9 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xAA */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xAB */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xAC */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xAD */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xAE */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xAF */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xB0 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xB1 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xB2 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xB3 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xB4 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xB5 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xB6 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xB7 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xB8 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xB9 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xBA */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xBB */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xBC */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xBD */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xBE */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xBF */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xC0 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xC1 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xC2 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xC3 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xC4 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xC5 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xC6 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xC7 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xC8 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xC9 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xCA */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xCB */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xCC */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xCD */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xCE */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xCF */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xD0 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xD1 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xD2 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xD3 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xD4 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xD5 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xD6 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xD7 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xD8 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xD9 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xDA */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xDB */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xDC */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xDD */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xDE */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xDF */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xE0 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xE1 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xE2 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xE3 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xE4 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xE5 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xE6 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xE7 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xE8 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xE9 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xEA */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xEB */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xEC */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xED */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xEE */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xEF */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xF0 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xF1 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xF2 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xF3 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xF4 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xF5 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xF6 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xF7 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xF8 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xF9 */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xFA */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xFB */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xFC */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xFD */
    lxb_css_syntax_state_ident_like_not_url_begin, /* 0xFE */
    lxb_css_syntax_state_ident_like_not_url_begin  /* 0xFF */
};


#endif /* LEXBOR_CSS_SYNTAX_STATE_RES_H */

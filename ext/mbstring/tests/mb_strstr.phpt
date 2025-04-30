--TEST--
mb_strstr()
--EXTENSIONS--
mbstring
--FILE--
<?php
function EUC_JP($utf8str) {
    return mb_convert_encoding($utf8str, "EUC-JP", "UTF-8");
}

function FROM_EUC_JP($eucjpstr) {
    return mb_convert_encoding($eucjpstr, "UTF-8", "EUC-JP");
}

var_dump(mb_strstr("あいうえおかきくけこ", "おかき"));
var_dump(mb_strstr("あいうえおかきくけこ", "おかき", false));
var_dump(mb_strstr("あいうえおかきくけこ", "おかき", true));
var_dump(FROM_EUC_JP(mb_strstr(EUC_JP("あいうえおかきくけこ"), EUC_JP("おかき"), false, "EUC-JP")));
var_dump(FROM_EUC_JP(mb_strstr(EUC_JP("あいうえおかきくけこ"), EUC_JP("おかき"), true, "EUC-JP")));
mb_internal_encoding("EUC-JP");
var_dump(FROM_EUC_JP(mb_strstr(EUC_JP("あいうえおかきくけこ"), EUC_JP("おかき"))));
var_dump(FROM_EUC_JP(mb_strstr(EUC_JP("あいうえおかきくけこ"), EUC_JP("おかき"), false)));
var_dump(FROM_EUC_JP(mb_strstr(EUC_JP("あいうえおかきくけこ"), EUC_JP("おかき"), true)));

// Regression test from when mb_strstr was being reimplemented
var_dump(bin2hex(mb_strstr("\xdd\x00", "", false, 'UTF-8')));
var_dump(bin2hex(mb_strstr("M\xff\xff\xff\x00", "\x00", false, "SJIS")));

// Test handling of invalid UTF-8 string
// Thanks to Stefan Schiller
var_dump(mb_strstr("\xf0start", "start", false, "UTF-8"));
var_dump(mb_strstr("\xf0start", "start", true, "UTF-8"));

?>
--EXPECT--
string(18) "おかきくけこ"
string(18) "おかきくけこ"
string(12) "あいうえ"
string(18) "おかきくけこ"
string(12) "あいうえ"
string(18) "おかきくけこ"
string(18) "おかきくけこ"
string(12) "あいうえ"
string(4) "3f00"
string(2) "00"
string(5) "start"
string(1) "?"

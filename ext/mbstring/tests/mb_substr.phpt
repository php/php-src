--TEST--
mb_substr()
--EXTENSIONS--
mbstring
--FILE--
<?php
ini_set('include_path','.');
include_once('common.inc');

// EUC-JP
$euc_jp = mb_convert_encoding('0123この文字列は日本語です。EUC-JPを使っています。日本語は面倒臭い。', 'EUC-JP', 'UTF-8');
// SJIS
$sjis = mb_convert_encoding('日本語テキストです。01234５６７８９。', 'SJIS', 'UTF-8');
// ISO-2022-JP
$iso2022jp = "\x1B\$B\x21\x21!r\x1B(BABC";
// GB-18030
$gb18030 = mb_convert_encoding('密码用户名密码名称名称', 'GB18030', 'UTF-8');
// HZ
$hz = "The next sentence is in GB.~{<:Ky2;S{#,NpJ)l6HK!#~}Bye.";
// UTF-8
$utf8 = "Greek: Σὲ γνωρίζω ἀπὸ τὴν κόψη Russian: Зарегистрируйтесь";
// UTF-32
$utf32 = mb_convert_encoding($utf8, 'UTF-32', 'UTF-8');
// UTF-7
$utf7 = mb_convert_encoding($utf8, 'UTF-7', 'UTF-8');

echo "EUC-JP:\n";
print "1: ". bin2hex(mb_substr($euc_jp, 10,  10, 'EUC-JP')) . "\n";
print "2: ". bin2hex(mb_substr($euc_jp,  0, 100, 'EUC-JP')) . "\n";

$str = mb_substr($euc_jp, 100, 10, 'EUC-JP');
print ($str === "") ? "3 OK\n" : "BAD: " . bin2hex($str) . "\n";

$str = mb_substr($euc_jp, -100, 10, 'EUC-JP');
print ($str !== "") ? "4 OK: " . bin2hex($str) . "\n" : "BAD: " . bin2hex($str) . "\n";

echo "SJIS:\n";
print "1: " . bin2hex(mb_substr($sjis, 0, 3, 'SJIS')) . "\n";
print "2: " . bin2hex(mb_substr($sjis, -1, null, 'SJIS')) . "\n";
print "3: " . bin2hex(mb_substr($sjis, -5, 3, 'SJIS')) . "\n";
print "4: " . bin2hex(mb_substr($sjis, 1, null, 'SJIS')) . "\n";
print "5:" . bin2hex(mb_substr($sjis, 10, 0, 'SJIS')) . "\n";
echo "-- Testing illegal SJIS byte 0x80 --\n";
print bin2hex(mb_substr("\x80abc\x80\xA1", 3, 2, 'SJIS')) . "\n";
print bin2hex(mb_substr("\x80abc\x80\xA1", 0, 3, 'SJIS')) . "\n";

echo "SJIS-2004:\n";
print bin2hex(mb_substr("\x80abc\x80\xA1", 3, 2, 'SJIS-2004')) . "\n";
print bin2hex(mb_substr("\x80abc\x80\xA1", 0, 3, 'SJIS-2004')) . "\n";

echo "MacJapanese:\n";
print bin2hex(mb_substr("\x80abc\x80\xA1", 3, 2, 'MacJapanese')) . "\n";
print bin2hex(mb_substr("\x80abc\x80\xA1", 0, 3, 'MacJapanese')) . "\n";

echo "SJIS-Mobile#DOCOMO:\n";
print bin2hex(mb_substr("\x80abc\x80\xA1", 3, 2, 'SJIS-Mobile#DOCOMO')) . "\n";
print bin2hex(mb_substr("\x80abc\x80\xA1", 0, 3, 'SJIS-Mobile#DOCOMO')) . "\n";

echo "SJIS-Mobile#KDDI:\n";
print bin2hex(mb_substr("\x80abc\x80\xA1", 3, 2, 'SJIS-Mobile#KDDI')) . "\n";
print bin2hex(mb_substr("\x80abc\x80\xA1", 0, 3, 'SJIS-Mobile#KDDI')) . "\n";

echo "SJIS-Mobile#SoftBank:\n";
print bin2hex(mb_substr("\x80abc\x80\xA1", 3, 2, 'SJIS-Mobile#SoftBank')) . "\n";
print bin2hex(mb_substr("\x80abc\x80\xA1", 0, 3, 'SJIS-Mobile#SoftBank')) . "\n";

echo "-- Testing MacJapanese characters which map to 3-5 codepoints each --\n";

/* There are many characters in MacJapanese which map to sequences of several codepoints */
print bin2hex(mb_substr("abc\x85\xAB\x85\xAC\x85\xAD", 0, 3, 'MacJapanese')) . "\n";
print bin2hex(mb_substr("abc\x85\xAB\x85\xAC\x85\xAD", 3, 2, 'MacJapanese')) . "\n";
print bin2hex(mb_substr("abc\x85\xAB\x85\xAC\x85\xAD", -2, 1, 'MacJapanese')) . "\n";
print bin2hex(mb_substr("abc\x85\xBF\x85\xC0\x85\xC1", 0, 3, 'MacJapanese')) . "\n";
print bin2hex(mb_substr("abc\x85\xBF\x85\xC0\x85\xC1", 3, 2, 'MacJapanese')) . "\n";
print bin2hex(mb_substr("abc\x85\xBF\x85\xC0\x85\xC1", -2, 1, 'MacJapanese')) . "\n";

echo "ISO-2022-JP:\n";
print "1: " . bin2hex(mb_substr($iso2022jp, 0, 3, 'ISO-2022-JP')) . "\n";
print "2: " . bin2hex(mb_substr($iso2022jp, -1, null, 'ISO-2022-JP')) . "\n";
print "3: " . bin2hex(mb_substr($iso2022jp, -6, 3, 'ISO-2022-JP')) . "\n";
print "4: " . bin2hex(mb_substr($iso2022jp, -3, 2, 'ISO-2022-JP')) . "\n";
print "5: " . bin2hex(mb_substr($iso2022jp, 1, null, 'ISO-2022-JP')) . "\n";
print "6:" . bin2hex(mb_substr($iso2022jp, 10, 0, 'ISO-2022-JP')) . "\n";
print "7:" . bin2hex(mb_substr($iso2022jp, 100, 10, 'ISO-2022-JP')) . "\n";

echo "GB-18030:\n";
print "1: " . bin2hex(mb_substr($gb18030, 0, 3, 'GB-18030')) . "\n";
print "2: " . bin2hex(mb_substr($gb18030, -1, null, 'GB-18030')) . "\n";
print "3: " . bin2hex(mb_substr($gb18030, -5, 3, 'GB-18030')) . "\n";
print "4: " . bin2hex(mb_substr($gb18030, 1, null, 'GB-18030')) . "\n";
print "5:" . bin2hex(mb_substr($gb18030, 10, 0, 'GB-18030')) . "\n";

echo "HZ:\n";
print "1: " . mb_substr($hz, 0, 3, 'HZ') . "\n";
print "2: " . mb_substr($hz, -1, null, 'HZ') . "\n";
print "3: " . mb_substr($hz, -5, 3, 'HZ') . "\n";
print "4: " . mb_substr($hz, 1, null, 'HZ') . "\n";
print "5:" . mb_substr($hz, 10, 0, 'HZ') . "\n";

echo "UTF-8:\n";
print "1: " . mb_substr($utf8, 0, 3, 'UTF-8') . "\n";
print "2: " . mb_substr($utf8, -1, null, 'UTF-8') . "\n";
print "3: " . mb_substr($utf8, -5, 3, 'UTF-8') . "\n";
print "4: " . mb_substr($utf8, 1, null, 'UTF-8') . "\n";
print "5:" . mb_substr($utf8, 10, 0, 'UTF-8') . "\n";

echo "UTF-32:\n";
print "1: " . mb_convert_encoding(mb_substr($utf32, 0, 3, 'UTF-32'), 'UTF-8', 'UTF-32') . "\n";
print "2: " . mb_convert_encoding(mb_substr($utf32, -1, null, 'UTF-32'), 'UTF-8', 'UTF-32') . "\n";
print "3: " . mb_convert_encoding(mb_substr($utf32, -5, 3, 'UTF-32'), 'UTF-8', 'UTF-32') . "\n";
print "4: " . mb_convert_encoding(mb_substr($utf32, 1, null, 'UTF-32'), 'UTF-8', 'UTF-32') . "\n";
print "5:" . mb_convert_encoding(mb_substr($utf32, 10, 0, 'UTF-32'), 'UTF-8', 'UTF-32') . "\n";

echo "UTF-7:\n";
print "1: " . mb_convert_encoding(mb_substr($utf7, 0, 3, 'UTF-7'), 'UTF-8', 'UTF-7') . "\n";
print "2: " . mb_convert_encoding(mb_substr($utf7, -1, null, 'UTF-7'), 'UTF-8', 'UTF-7') . "\n";
print "3: " . mb_convert_encoding(mb_substr($utf7, -5, 3, 'UTF-7'), 'UTF-8', 'UTF-7') . "\n";
print "4: " . mb_convert_encoding(mb_substr($utf7, 1, null, 'UTF-7'), 'UTF-8', 'UTF-7') . "\n";
print "5:" . mb_convert_encoding(mb_substr($utf7, 10, 0, 'UTF-7'), 'UTF-8', 'UTF-7') . "\n";

echo "Testing agreement with mb_strpos on invalid UTF-8 string:\n";
/* Stefan Schiller pointed out that on invalid UTF-8 strings, character indices returned
 * by mb_strpos would not extract the desired part of the string when passed to mb_substr.
 * This is the test case which he provided: */
$data = "\xF0AAA<b>";
$pos = mb_strpos($data, "<", 0, "UTF-8");
$out = mb_substr($data, 0, $pos, "UTF-8");
print $out . "\n";

echo "Regression:\n";
/* During development, one >= comparison in mb_get_substr was wrongly written as >
 * This was caught by libFuzzer */
$str = "\xbd\xbd\xbd\xbd\xbd\xbd\xbd\xbe\xbd\xbd\xbd\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x89\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x00\x00\x00\x00\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8b\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8f\x8b\x8b\x8b\xbd\xbd\xbd\xbd\xbd\xbd\xbd\xbe\x01:O\xaa\xd3";
echo bin2hex(mb_substr($str, 0, 128, "JIS")), "\n";

?>
--EXPECT--
EUC-JP:
1: c6fccbdcb8eca4c7a4b9a1a34555432d
2: 30313233a4b3a4cecab8bbfacef3a4cfc6fccbdcb8eca4c7a4b9a1a34555432d4a50a4f2bbc8a4c3a4c6a4a4a4dea4b9a1a3c6fccbdcb8eca4cfccccc5ddbdada4a4a1a3
3 OK
4 OK: 30313233a4b3a4cecab8bbfacef3a4cf
SJIS:
1: 93fa967b8cea
2: 8142
3: 825582568257
4: 967b8cea8365834c8358836782c582b781423031323334825482558256825782588142
5:
-- Testing illegal SJIS byte 0x80 --
633f
3f6162
SJIS-2004:
633f
3f6162
MacJapanese:
6380
806162
SJIS-Mobile#DOCOMO:
633f
3f6162
SJIS-Mobile#KDDI:
633f
3f6162
SJIS-Mobile#SoftBank:
633f
3f6162
-- Testing MacJapanese characters which map to 3-5 codepoints each --
616263
3f3f
58
616263
3f3f
78
ISO-2022-JP:
1: 1b2442212121721b284241
2: 43
3: 1b2442212121721b284241
4: 4142
5: 1b244221721b2842414243
6:
7:
GB-18030:
1: c3dcc2ebd3c3
2: b3c6
3: c2ebc3fbb3c6
4: c2ebd3c3bba7c3fbc3dcc2ebc3fbb3c6c3fbb3c6
5:
HZ:
1: The
2: .
3: ~{!#~}By
4: he next sentence is in GB.~{<:Ky2;S{#,NpJ)l6HK!#~}Bye.
5:
UTF-8:
1: Gre
2: ь
3: йте
4: reek: Σὲ γνωρίζω ἀπὸ τὴν κόψη Russian: Зарегистрируйтесь
5:
UTF-32:
1: Gre
2: ь
3: йте
4: reek: Σὲ γνωρίζω ἀπὸ τὴν κόψη Russian: Зарегистрируйтесь
5:
UTF-7:
1: Gre
2: ь
3: йте
4: reek: Σὲ γνωρίζω ἀπὸ τὴν κόψη Russian: Зарегистрируйтесь
5:
Testing agreement with mb_strpos on invalid UTF-8 string:
?AAA
Regression:
1b28493d3d3d3d3d3d3d3e3d3d3d1b28423f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f000000003f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f3f1b28493d3d3d3d3d3d3d3e1b2842013a4f1b28492a1b2842

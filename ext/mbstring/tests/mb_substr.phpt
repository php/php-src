--TEST--
mb_substr()
--EXTENSIONS--
mbstring
--FILE--
<?php
ini_set('include_path','.');
include_once('common.inc');

// EUC-JP
$euc_jp = "0123\xA4\xB3\xA4\xCE\xCA\xB8\xBB\xFA\xCE\xF3\xA4\xCF\xC6\xFC\xCB\xDC\xB8\xEC\xA4\xC7\xA4\xB9\xA1\xA3EUC-JP\xA4\xF2\xBB\xC8\xA4\xC3\xA4\xC6\xA4\xA4\xA4\xDE\xA4\xB9\xA1\xA3\xC6\xFC\xCB\xDC\xB8\xEC\xA4\xCF\xCC\xCC\xC5\xDD\xBD\xAD\xA4\xA4\xA1\xA3";
// SJIS
$sjis = "\x93\xFA\x96{\x8C\xEA\x83e\x83L\x83X\x83g\x82\xC5\x82\xB7\x81B01234\x82T\x82U\x82V\x82W\x82X\x81B";
// ISO-2022-JP
$iso2022jp = "\x1B\$B\x21\x21!r\x1B(BABC";
// GB-18030
$gb18030 = "\xC3\xDC\xC2\xEB\xD3\xC3\xBB\xA7\xC3\xFB\xC3\xDC\xC2\xEB\xC3\xFB\xB3\xC6\xC3\xFB\xB3\xC6";
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

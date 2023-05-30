--TEST--
mb_strlen()
--EXTENSIONS--
mbstring
--FILE--
<?php
// TODO: Add more encodings

ini_set('include_path', __DIR__);
include_once('common.inc');

// restore detect_order to 'auto'
mb_detect_order('auto');

// Test string
$euc_jp = mb_convert_encoding("0123この文字列は日本語です。EUC-JPを使っています。0123日本語は面倒臭い。", 'EUC-JP', 'UTF-8');
$ascii  = 'abcdefghijklmnopqrstuvwxyz;]=#0123456789';

echo "== ASCII ==\n";
print mb_strlen($ascii,'ASCII') . "\n";
print strlen($ascii) . "\n";

echo "== EUC-JP ==\n";
print mb_strlen($euc_jp,'EUC-JP') . "\n";
mb_internal_encoding('EUC-JP') or print("mb_internal_encoding() failed\n");
print strlen($euc_jp) . "\n";

echo "== SJIS ==\n";
$sjis = mb_convert_encoding($euc_jp, 'SJIS','EUC-JP');
print mb_strlen($sjis,'SJIS') . "\n";
mb_internal_encoding('SJIS') or print("mb_internal_encoding() failed\n");
print strlen($sjis) . "\n";
print "-- Testing illegal bytes 0x80,0xFD-FF --\n";
// mb_strlen used to wrongly treat 0x80 as the starting byte of a 2-byte SJIS character
print mb_strlen("\x80\xA1", 'SJIS') . "\n";
print mb_strlen("abc\xFD\xFE\xFF", 'SJIS') . "\n";

echo "== CP932 ==\n";
print mb_strlen("\x80\xA1", "CP932") . "\n";
// 0xFD, 0xFE, 0xFF is reserved.
print mb_strlen("abc\xFD\xFE\xFF", 'CP932') . "\n";
print mb_strlen("\x80\xA1", "SJIS-win") . "\n";
print mb_strlen("abc\xFD\xFE\xFF", 'SJIS-win') . "\n";

echo "== MacJapanese ==\n";
print mb_strlen("\x80\xA1", 'MacJapanese') . "\n";
print mb_strlen("abc\xFD\xFE\xFF", 'MacJapanese') . "\n";

echo "== SJIS-2004 ==\n";
print mb_strlen("\x80\xA1", 'SJIS-2004') . "\n";
print mb_strlen("abc\xFD\xFE\xFF", 'SJIS-2004') . "\n";

echo "== SJIS-Mobile#DOCOMO ==\n";
print mb_strlen("\x80\xA1", 'SJIS-Mobile#DOCOMO') . "\n";
print mb_strlen("abc\xFD\xFE\xFF", 'SJIS-Mobile#DOCOMO') . "\n";

echo "== SJIS-Mobile#KDDI ==\n";
print mb_strlen("\x80\xA1", 'SJIS-Mobile#KDDI') . "\n";
print mb_strlen("abc\xFD\xFE\xFF", 'SJIS-Mobile#KDDI') . "\n";

echo "== SJIS-Mobile#SoftBank ==\n";
print mb_strlen("\x80\xA1", 'SJIS-Mobile#SoftBank') . "\n";
print mb_strlen("abc\xFD\xFE\xFF", 'SJIS-Mobile#SoftBank') . "\n";

echo "== JIS ==\n";
$jis = mb_convert_encoding($euc_jp, 'JIS','EUC-JP');
print mb_strlen($jis,'JIS') . "\n";
mb_internal_encoding('JIS')  or print("mb_internal_encoding() failed\n");
print strlen($jis) . "\n";

echo "== UTF-8 ==\n";
$utf8 = mb_convert_encoding($euc_jp, 'UTF-8', 'EUC-JP');
print mb_strlen($utf8,'UTF-8') . " codepoints\n";
mb_internal_encoding('UTF-8') or print("mb_internal_encoding() failed\n");
print strlen($utf8) . " bytes\n";

$utf8 = "abcde あいうえお 汉字 ελληνικά";
$long_utf8 = str_repeat($utf8, 100);
print mb_strlen($utf8, 'UTF-8') . "\n";
print mb_strlen($long_utf8, 'UTF-8') . "\n";

echo "== UTF-8 with performance optimizations ==\n";
// Optimized mb_strlen can be used on UTF-8 strings after they are checked for validity
mb_check_encoding($utf8);
mb_check_encoding($long_utf8);
print mb_strlen($utf8, 'UTF-8') . "\n";
print mb_strlen($long_utf8, 'UTF-8') . "\n";

$str = str_repeat('Σ', 2048); // 2-byte UTF-8 character
mb_check_encoding($str, 'UTF-8');
print mb_strlen($str, 'UTF-8') . "\n";

// Wrong Parameters
echo "== WRONG PARAMETERS ==\n";
// Wrong encoding
mb_internal_encoding('EUC-JP');
try {
    var_dump( mb_strlen($euc_jp, 'BAD_NAME') );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
== ASCII ==
40
40
== EUC-JP ==
43
72
== SJIS ==
43
72
-- Testing illegal bytes 0x80,0xFD-FF --
2
6
== CP932 ==
2
6
2
6
== MacJapanese ==
2
7
== SJIS-2004 ==
2
6
== SJIS-Mobile#DOCOMO ==
2
6
== SJIS-Mobile#KDDI ==
2
6
== SJIS-Mobile#SoftBank ==
2
6
== JIS ==
43
90
== UTF-8 ==
43 codepoints
101 bytes
23
2300
== UTF-8 with performance optimizations ==
23
2300
2048
== WRONG PARAMETERS ==
mb_strlen(): Argument #2 ($encoding) must be a valid encoding, "BAD_NAME" given

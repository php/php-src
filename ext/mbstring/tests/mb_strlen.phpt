--TEST--
mb_strlen()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
// TODO: Add more encodings

ini_set('include_path', __DIR__);
include_once('common.inc');

// restore detect_order to 'auto'
mb_detect_order('auto');

// Test string
$euc_jp = '0123この文字列は日本語です。EUC-JPを使っています。0123日本語は面倒臭い。';
$ascii  = 'abcdefghijklmnopqrstuvwxyz;]=#0123456789';

// ASCII
echo "== ASCII ==\n";
print  mb_strlen($ascii,'ASCII') . "\n";
print  strlen($ascii) . "\n";

// EUC-JP
echo "== EUC-JP ==\n";
print  mb_strlen($euc_jp,'EUC-JP') . "\n";
mb_internal_encoding('EUC-JP') or print("mb_internal_encoding() failed\n");
print  strlen($euc_jp) . "\n";

// SJIS
echo "== SJIS ==\n";
$sjis = mb_convert_encoding($euc_jp, 'SJIS','EUC-JP');
print  mb_strlen($sjis,'SJIS') . "\n";
mb_internal_encoding('SJIS') or print("mb_internal_encoding() failed\n");
print  strlen($sjis) . "\n";

// JIS
// Note: either convert_encoding or strlen has problem
echo "== JIS ==\n";
$jis = mb_convert_encoding($euc_jp, 'JIS','EUC-JP');
print  mb_strlen($jis,'JIS') . "\n";
mb_internal_encoding('JIS')  or print("mb_internal_encoding() failed\n");
print  strlen($jis) . "\n";

// UTF-8
// Note: either convert_encoding or strlen has problem
echo "== UTF-8 ==\n";
$utf8 = mb_convert_encoding($euc_jp, 'UTF-8','EUC-JP');
print  mb_strlen($utf8,'UTF-8') . "\n";
mb_internal_encoding('UTF-8')  or print("mb_internal_encoding() failed\n");
print  strlen($utf8) . "\n";


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
== JIS ==
43
90
== UTF-8 ==
43
101
== WRONG PARAMETERS ==
mb_strlen(): Argument #2 ($encoding) must be a valid encoding, "BAD_NAME" given

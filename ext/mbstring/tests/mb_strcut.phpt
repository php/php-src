--TEST--
mb_strcut()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
// TODO: Add more encodings
ini_set('include_path','.');
include_once('common.inc');

// EUC-JP
$euc_jp = '0123この文字列は日本語です。EUC-JPを使っています。日本語は面倒臭い。';

print  mb_strcut($euc_jp,  6,   5,'EUC-JP') . "\n";
print  mb_strcut($euc_jp,  0, 100,'EUC-JP') . "\n";

$str = mb_strcut($euc_jp, 100, 10,'EUC-JP');
($str === "") ? print "OK\n" : print "NG: $str\n";

$str = mb_strcut($euc_jp, -100, 10,'EUC-JP');
($str !== "") ?	print "OK: $str\n" : print "NG:\n";


?>

--EXPECT--
の文
0123この文字列は日本語です。EUC-JPを使っています。日本語は面倒臭い。
OK
OK: 0123この文



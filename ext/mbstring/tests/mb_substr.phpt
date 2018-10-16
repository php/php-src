--TEST--
mb_substr()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
mbstring.func_overload=0
--FILE--
<?php
// TODO: Add more encodings
ini_set('include_path','.');
include_once('common.inc');

// EUC-JP
$euc_jp = '0123この文字列は日本語です。EUC-JPを使っています。日本語は面倒臭い。';

print  "1: ". bin2hex(mb_substr($euc_jp,  10,  10,'EUC-JP')) . "\n";
print  "2: ". bin2hex(mb_substr($euc_jp,   0, 100,'EUC-JP')) . "\n";

$str = mb_substr($euc_jp, 100, 10,'EUC-JP');
// Note: returns last character
($str === "") ? print "3 OK\n" : print "NG: ".bin2hex($str)."\n";

$str = mb_substr($euc_jp, -100, 10,'EUC-JP');
($str !== "") ? print "4 OK: ".bin2hex($str)."\n" : print "NG: ".bin2hex($str)."\n";

?>
--EXPECT--
1: c6fccbdcb8eca4c7a4b9a1a34555432d
2: 30313233a4b3a4cecab8bbfacef3a4cfc6fccbdcb8eca4c7a4b9a1a34555432d4a50a4f2bbc8a4c3a4c6a4a4a4dea4b9a1a3c6fccbdcb8eca4cfccccc5ddbdada4a4a1a3
3 OK
4 OK: 30313233a4b3a4cecab8bbfacef3a4cf

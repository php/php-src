--TEST--
mb_strwidth() & mb_strwidth() - unknown encoding
--CREDITS--
Yasuo Ohgaki
Jachim Coudenys <jachimcoudenys@gmail.com>
User Group: PHP-WVL & PHPGent #PHPTestFest
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
// TODO: Add more encoding, strings.....
//$debug = true;
ini_set('include_path', dirname(__FILE__));
include_once('common.inc');

// EUC-JP
$euc_jp = '0123この文字列は日本語です。EUC-JPを使っています。日本語は面倒臭い。';

print  "1: ". mb_strwidth($euc_jp, 'EUC-JP') . "\n";
/*

print  "2: ". mb_strwidth($euc_jp, 'EUC-JP') . "\n";
print  "3: ". mb_strwidth($euc_jp, 'EUC-JP') . "\n";
// Note: Did not start form -22 offset. Staring from 0.
print  "4: ". mb_strwidth($euc_jp, 'EUC-JP') . "\n";

$str = mb_strwidth($euc_jp, 100, -10,'...','EUC-JP');
($str === "") ? print "5 OK\n" : print "NG: $str\n";

$str = mb_strwidth($euc_jp, -100, 10,'...','EUC-JP');
($str !== "") ?	print "6 OK: $str\n" : print "NG: $str\n";
*/

mb_strwidth('coudenys', 'jachim');
?>
--EXPECTF--
1: 68
Warning: mb_strwidth(): Unknown encoding "jachim" in %s on line %d

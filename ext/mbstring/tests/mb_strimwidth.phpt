--TEST--
mb_strimwidth()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
--FILE--
<?php
// TODO: Add more encoding
//$debug = true;
ini_set('include_path', dirname(__FILE__));
include_once('common.inc');

// EUC-JP
$euc_jp = '0123この文字列は日本語です。EUC-JPを使っています。日本語は面倒臭い。';

print  "1: ". mb_strimwidth($euc_jp,  0, 15,'...','EUC-JP') . "\n";
print  "2: ". mb_strimwidth($euc_jp,  0, 100,'...','EUC-JP') . "\n";
print  "3: ". mb_strimwidth($euc_jp, 15, 100,'...','EUC-JP') . "\n";
// Note: Did not start form -22 offset. Staring from 0.
$str = mb_strimwidth($euc_jp,-22, 100,'...','EUC-JP');
($str === FALSE) ? print "4 OK\n" : print "NG: $str\n";

$str = mb_strimwidth($euc_jp, 100, -10,'...','EUC-JP');
($str === FALSE) ? print "5 OK\n" : print "NG: $str\n";

$str = mb_strimwidth($euc_jp, -100, 10,'...','EUC-JP');
($str === FALSE) ? print "6 OK\n" : print "NG: $str\n";

?>

--EXPECT--
1: 0123この文字...
2: 0123この文字列は日本語です。EUC-JPを使っています。日本語は面倒臭い。
3: 。EUC-JPを使っています。日本語は面倒臭い。
ERR: Warning
4 OK
ERR: Warning
5 OK
ERR: Warning
6 OK



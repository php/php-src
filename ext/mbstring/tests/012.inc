<?php
// TODO: Add more encodings
ini_set('include_path','.');
include_once('common.inc');

// EUC-JP
$euc_jp = '0123この文字列は日本語です。EUC-JPを使っています。日本語は面倒臭い。';

print  "1: ". mb_substr($euc_jp,  10,  10,'EUC-JP') . "\n";
print  "2: ". mb_substr($euc_jp,   0, 100,'EUC-JP') . "\n";

$str = mb_substr($euc_jp, 100, 10,'EUC-JP');
// Note: returns last character
($str === "") ? print "3 OK\n" : print "NG: $str\n";

$str = mb_substr($euc_jp, -100, 10,'EUC-JP');
($str !== "") ? print "4 OK: $str\n" : print "NG: $str\n";

?>

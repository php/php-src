--TEST--
mb_strimwidth()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
--FILE--
<?php
// TODO: Add more encoding

// EUC-JP
$euc_jp = '0123この文字列は日本語です。EUC-JPを使っています。日本語は面倒臭い。';

print "String width: ".mb_strwidth($euc_jp,'EUC-JP')."\n";

print  "1: ". mb_strimwidth($euc_jp,  0, 15,'...','EUC-JP') . "\n";
print  "2: ". mb_strimwidth($euc_jp,  0, 100,'...','EUC-JP') . "\n";
print  "3: ". mb_strimwidth($euc_jp, 15, 100,'...','EUC-JP') . "\n";
print  "4: ". mb_strimwidth($euc_jp, -30, 5,'...','EUC-JP') . "\n";
print  "5: ". mb_strimwidth($euc_jp, 38, 5,'...','EUC-JP') . "\n";
print  "6: ". mb_strimwidth($euc_jp, 38, -25,'...','EUC-JP') . "\n";
print  "7: ". mb_strimwidth($euc_jp, -30, -25,'...','EUC-JP') . "\n";

try {
    var_dump(mb_strimwidth($euc_jp, 0, -100,'...','EUC-JP'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_strimwidth($euc_jp, 100, 10,'...','EUC-JP'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_strimwidth($euc_jp, -100, 10,'...','EUC-JP'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_strimwidth($euc_jp, -10, -12,'...','EUC-JP'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
String width: 68
1: 0123この文字...
2: 0123この文字列は日本語です。EUC-JPを使っています。日本語は面倒臭い。
3: 。EUC-JPを使っています。日本語は面倒臭い。
4: 。
5: 。
6: 。
7: 。
mb_strimwidth(): Argument #3 ($width) is out of range
mb_strimwidth(): Argument #2 ($start) is out of range
mb_strimwidth(): Argument #2 ($start) is out of range
mb_strimwidth(): Argument #3 ($width) is out of range

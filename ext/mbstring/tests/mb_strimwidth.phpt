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
$euc_jp = '0123����ʸ��������ܸ�Ǥ���EUC-JP��ȤäƤ��ޤ������ܸ�����ݽ�����';

print "String width: ".mb_strwidth($euc_jp,'EUC-JP')."\n";

print  "1: ". mb_strimwidth($euc_jp,  0, 15,'...','EUC-JP') . "\n";
print  "2: ". mb_strimwidth($euc_jp,  0, 100,'...','EUC-JP') . "\n";
print  "3: ". mb_strimwidth($euc_jp, 15, 100,'...','EUC-JP') . "\n";
print  "4: ". mb_strimwidth($euc_jp, -30, 5,'...','EUC-JP') . "\n";
print  "5: ". mb_strimwidth($euc_jp, 38, 5,'...','EUC-JP') . "\n";
print  "6: ". mb_strimwidth($euc_jp, 38, -25,'...','EUC-JP') . "\n";
print  "7: ". mb_strimwidth($euc_jp, -30, -25,'...','EUC-JP') . "\n";

$str = mb_strimwidth($euc_jp, 0, -100,'...','EUC-JP');
($str === FALSE) ? print "10 OK\n" : print "NG: $str\n";

$str = mb_strimwidth($euc_jp, 100, 10,'...','EUC-JP');
($str === FALSE) ? print "11 OK\n" : print "NG: $str\n";

$str = mb_strimwidth($euc_jp, -100, 10,'...','EUC-JP');
($str === FALSE) ? print "12 OK\n" : print "NG: $str\n";

$str = mb_strimwidth($euc_jp, -10, -12,'...','EUC-JP');
($str === FALSE) ? print "13 OK\n" : print "NG: $str\n";

?>
--EXPECTF--
String width: 68
1: 0123����ʸ��...
2: 0123����ʸ��������ܸ�Ǥ���EUC-JP��ȤäƤ��ޤ������ܸ�����ݽ�����
3: ��EUC-JP��ȤäƤ��ޤ������ܸ�����ݽ�����
4: ��
5: ��
6: ��
7: ��

Warning: mb_strimwidth(): Width is out of range in %s on line %d
10 OK

Warning: mb_strimwidth(): Start position is out of range in %s on line %d
11 OK

Warning: mb_strimwidth(): Start position is out of range in %s on line %d
12 OK

Warning: mb_strimwidth(): Width is out of range in %s on line %d
13 OK

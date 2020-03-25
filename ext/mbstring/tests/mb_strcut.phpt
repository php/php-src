--TEST--
mb_strcut()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
--FILE--
<?php
// TODO: Add more encodings
ini_set('include_path', __DIR__);
include_once('common.inc');

// EUC-JP
$euc_jp = '0123����ʸ��������ܸ�Ǥ���EUC-JP��ȤäƤ��ޤ������ܸ�����ݽ�����';

print  mb_strcut($euc_jp,  6,   5,'EUC-JP') . "\n";
print  mb_strcut($euc_jp,  0, 100,'EUC-JP') . "\n";

$str = mb_strcut($euc_jp, 100, 10,'EUC-JP');
($str === false) ? print "OK\n" : print "NG: $str\n";

$str = mb_strcut($euc_jp, -100, 10,'EUC-JP');
($str !== "") ?	print "OK: $str\n" : print "NG:\n";


?>
--EXPECT--
��ʸ
0123����ʸ��������ܸ�Ǥ���EUC-JP��ȤäƤ��ޤ������ܸ�����ݽ�����
OK
OK: 0123����ʸ

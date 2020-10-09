--TEST--
mb_strwidth()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
ini_set('include_path', __DIR__);
include_once('common.inc');

// EUC-JP
$euc_jp  = pack('H*', '30313233a4b3a4cecab8bbfacef3a4cfc6fccbdcb8eca4c7a4b9a1a34555432d4a50a4f2bbc8a4c3a4c6a4a4a4dea4b9a1a3c6fccbdcb8eca4cfccccc5ddbdada4a4a1a3');
// UTF-8
$utf8    = '∮ E⋅da = Q';
// UTF-16LE
$utf16le = pack('H*', '1a043804400438043b043b04380446043004200069007300200043007900720069006c006c0069006300');
// UTF-16BE
$utf16be = pack('H*', '041a043804400438043b043b04380446043000200069007300200043007900720069006c006c00690063');
// KOI8-R
$koi8r   = pack('H*', 'ebc9d2c9ccccc9c3c120697320437972696c6c6963');

print "1: " . mb_strwidth($euc_jp,  'EUC-JP')   . "\n";
print "2: " . mb_strwidth($utf8,    'UTF-8')    . "\n";
print "3: " . mb_strwidth($utf16le, 'UTF-16LE') . "\n";
print "4: " . mb_strwidth($utf16be, 'UTF-16BE') . "\n";
print "5: " . mb_strwidth($koi8r,   'KOI8-R')   . "\n";

?>
--EXPECT--
1: 68
2: 10
3: 21
4: 21
5: 21

--TEST--
mb_strcut()
--EXTENSIONS--
mbstring
--INI--
output_handler=
--FILE--
<?php
function MBStringChars($string, $encoding) {
  $chars = mb_str_split($string, 1, $encoding);
  return '[' . implode(' ', array_map(function($char) {
    return join(unpack('H*', $char));
  }, $chars)) . ']';
}

ini_set('include_path', __DIR__);
include_once('common.inc');

// EUC-JP
$euc_jp = pack('H*', '30313233a4b3a4cecab8bbfacef3a4cfc6fccbdcb8eca4c7a4b9a1a34555432d4a50a4f2bbc8a4c3a4c6a4a4a4dea4b9a1a3c6fccbdcb8eca4cfccccc5ddbdada4a4a1a3');
// UTF-8
$utf8    = pack('H*', 'e288ae2045e28b856461203d2051'); // has 2 multi-byte characters: [e288ae 20 45 e28b85 64 61 20 3d 20 51]
// UTF-16LE
$utf16le = pack('H*', '1a043804400438043b043b04380446043004200069007300200043007900720069006c006c0069006300');

print "== EUC-JP ==\n";
print MBStringChars(mb_strcut($euc_jp,  6,   5,'EUC-JP'), 'EUC-JP') . "\n";
print MBStringChars(mb_strcut($euc_jp,  5,   5,'EUC-JP'), 'EUC-JP') . "\n";
print MBStringChars(mb_strcut($euc_jp,  0, 100,'EUC-JP'), 'EUC-JP') . "\n";

$str = mb_strcut($euc_jp, 100, 10,'EUC-JP');
($str === "") ? print "OK\n" : print "No good\n";

$str = mb_strcut($euc_jp, -100, 10,'EUC-JP');
($str !== "") ?	print "OK\n" : print "No good\n";

print "== UTF-8 ==\n";
print MBStringChars(mb_strcut($utf8, 0, 0, 'UTF-8'), 'UTF-8') . "\n";
print MBStringChars(mb_strcut($utf8, 0, 1, 'UTF-8'), 'UTF-8') . "\n";
print MBStringChars(mb_strcut($utf8, 0, 2, 'UTF-8'), 'UTF-8') . "\n";
print MBStringChars(mb_strcut($utf8, 0, 3, 'UTF-8'), 'UTF-8') . "\n";
print MBStringChars(mb_strcut($utf8, 0, 4, 'UTF-8'), 'UTF-8') . "\n";
print MBStringChars(mb_strcut($utf8, 0, 5, 'UTF-8'), 'UTF-8') . "\n";
print MBStringChars(mb_strcut($utf8, 1, 2, 'UTF-8'), 'UTF-8') . "\n";
print MBStringChars(mb_strcut($utf8, 1, 3, 'UTF-8'), 'UTF-8') . "\n";
print MBStringChars(mb_strcut($utf8, 1, 4, 'UTF-8'), 'UTF-8') . "\n";

print "== UTF-16LE ==\n";
print MBStringChars(mb_strcut($utf16le, 0, 0, 'UTF-16LE'), 'UTF-16LE') . "\n";
print MBStringChars(mb_strcut($utf16le, 0, 1, 'UTF-16LE'), 'UTF-16LE') . "\n";
print MBStringChars(mb_strcut($utf16le, 0, 2, 'UTF-16LE'), 'UTF-16LE') . "\n";
print MBStringChars(mb_strcut($utf16le, 0, 3, 'UTF-16LE'), 'UTF-16LE') . "\n";
print MBStringChars(mb_strcut($utf16le, 1, 2, 'UTF-16LE'), 'UTF-16LE') . "\n";
print MBStringChars(mb_strcut($utf16le, 1, 3, 'UTF-16LE'), 'UTF-16LE') . "\n";
print MBStringChars(mb_strcut($utf16le, 1, 4, 'UTF-16LE'), 'UTF-16LE') . "\n";

?>
--EXPECT--
== EUC-JP ==
[a4ce cab8]
[a4b3 a4ce]
[30 31 32 33 a4b3 a4ce cab8 bbfa cef3 a4cf c6fc cbdc b8ec a4c7 a4b9 a1a3 45 55 43 2d 4a 50 a4f2 bbc8 a4c3 a4c6 a4a4 a4de a4b9 a1a3 c6fc cbdc b8ec a4cf cccc c5dd bdad a4a4 a1a3]
OK
OK
== UTF-8 ==
[]
[]
[]
[e288ae]
[e288ae 20]
[e288ae 20 45]
[]
[e288ae]
[e288ae 20]
== UTF-16LE ==
[]
[]
[1a04]
[1a04]
[1a04]
[1a04]
[1a04 3804]

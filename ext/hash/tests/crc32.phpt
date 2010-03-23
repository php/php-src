--TEST--
CRC32
--SKIPIF--
<?php extension_loaded('hash') or die('skip'); ?>
--FILE--
<?php
echo hash('crc32', ''), "\n";
echo hash('crc32', 'a'), "\n";
echo hash('crc32', 'abc'), "\n";
echo hash('crc32', 'message digest'), "\n";
echo hash('crc32', 'abcdefghijklmnopqrstuvwxyz'), "\n";
echo hash('crc32', 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789'), "\n";
echo hash('crc32', '12345678901234567890123456789012345678901234567890123456789012345678901234567890'), "\n";
echo hash('crc32b', ''), "\n";
echo hash('crc32b', 'a'), "\n";
echo hash('crc32b', 'abc'), "\n";
echo hash('crc32b', 'message digest'), "\n";
echo hash('crc32b', 'abcdefghijklmnopqrstuvwxyz'), "\n";
echo hash('crc32b', 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789'), "\n";
echo hash('crc32b', '12345678901234567890123456789012345678901234567890123456789012345678901234567890'), "\n";
?>
--EXPECT--
00000000
6b9b9319
73bb8c64
5703c9bf
9693bf77
882174a0
96790816
00000000
e8b7be43
352441c2
20159d7f
4c2750bd
1fc2e6d2
7ca94a72

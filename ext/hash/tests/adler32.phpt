--TEST--
ADLER32
--SKIPIF--
<?php extension_loaded('hash') or die('skip'); ?>
--FILE--
<?php
echo hash('adler32', ''), "\n";
echo hash('adler32', 'a'), "\n";
echo hash('adler32', 'abc'), "\n";
echo hash('adler32', 'message digest'), "\n";
echo hash('adler32', 'abcdefghijklmnopqrstuvwxyz'), "\n";
echo hash('adler32', 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789'), "\n";
echo hash('adler32', '12345678901234567890123456789012345678901234567890123456789012345678901234567890'), "\n";
?>
--EXPECT--
00000001
00620062
024d0127
29750586
90860b20
8adb150c
97b61069

--TEST--
md5() with RAW output
--FILE--
<?php
echo bin2hex(md5("", TRUE))."\n";
echo bin2hex(md5("a", TRUE))."\n";
echo bin2hex(md5("abc", TRUE))."\n";
echo bin2hex(md5("message digest", TRUE))."\n";
echo bin2hex(md5("abcdefghijklmnopqrstuvwxyz", TRUE))."\n";
echo bin2hex(md5("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", TRUE))."\n";
echo bin2hex(md5("12345678901234567890123456789012345678901234567890123456789012345678901234567890", TRUE))."\n";
?>
--EXPECT--
d41d8cd98f00b204e9800998ecf8427e
0cc175b9c0f1b6a831c399e269772661
900150983cd24fb0d6963f7d28e17f72
f96b697d7cb7938d525a2f31aaf161d0
c3fcd3d76192e4007dfb496cca67e13b
d174ab98d277d9f5a5611c2c9f419d9f
57edf4a22be3c955ac49da2e2107b67a

--TEST--
MD5 / Base64
--FILE--
<?php
function test($str) {
	$res = md5(base64_decode(base64_encode($str)))."\n";
	return $res;
} 
echo test("");
echo test("a");
echo test("abc");
echo test("message digest");
echo test("abcdefghijklmnopqrstuvwxyz");
echo test("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
echo test("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
?>
--EXPECT--
d41d8cd98f00b204e9800998ecf8427e
0cc175b9c0f1b6a831c399e269772661
900150983cd24fb0d6963f7d28e17f72
f96b697d7cb7938d525a2f31aaf161d0
c3fcd3d76192e4007dfb496cca67e13b
d174ab98d277d9f5a5611c2c9f419d9f
57edf4a22be3c955ac49da2e2107b67a

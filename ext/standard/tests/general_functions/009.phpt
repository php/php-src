--TEST--
SHA1
--POST--
--GET--
--FILE--
<?php
function test($str) {
	$res = sha1($str)."\n";
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
da39a3ee5e6b4b0d3255bfef95601890afd80709
86f7e437faa5a7fce15d1ddcb9eaeaea377667b8
a9993e364706816aba3e25717850c26c9cd0d89d
c12252ceda8be8994d5fa0290a47231c1d16aae3
32d10c7b8cf96570ca04ce37f2a19d84240d3a89
761c457bf73b14d27e9e9265c46f4b4dda11f940
50abf5706a150990a08b2c5ea40fa0e585554732

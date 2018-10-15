--TEST--
Test strval() function : usage variations  - Pass all valid char codes
--FILE--
<?php
/* Prototype  : string strval  ( mixed $var  )
 * Description: Get the string value of a variable.
 * Source code: ext/standard/string.c
 */

echo "*** Testing strval() : usage variations  - Pass all valid char codes ***\n";

$s0 = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f";
$s1 = "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f";
$s2 = "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f";
$s3 = "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f";
$s4 = "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f";
$s5 = "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f";
$s6 = "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f";
$s7 = "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f";
$s8 = "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f";
$s9 = "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f";
$sa = "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf";
$sb = "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf";
$sc = "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf";
$sd = "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf";
$se = "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef";
$sf = "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";

echo bin2hex(strval($s0));
echo "\n";
echo bin2hex(strval($s1));
echo "\n";
echo bin2hex(strval($s2));
echo "\n";
echo bin2hex(strval($s3));
echo "\n";
echo bin2hex(strval($s4));
echo "\n";
echo bin2hex(strval($s5));
echo "\n";
echo bin2hex(strval($s6));
echo "\n";
echo bin2hex(strval($s7));
echo "\n";
echo bin2hex(strval($s8));
echo "\n";
echo bin2hex(strval($s9));
echo "\n";
echo bin2hex(strval($sa));
echo "\n";
echo bin2hex(strval($sb));
echo "\n";
echo bin2hex(strval($sc));
echo "\n";
echo bin2hex(strval($sd));
echo "\n";
echo bin2hex(strval($se));
echo "\n";
echo bin2hex(strval($sf));
echo "\n";

?>
===DONE===
--EXPECTF--
*** Testing strval() : usage variations  - Pass all valid char codes ***
000102030405060708090a0b0c0d0e0f
101112131415161718191a1b1c1d1e1f
202122232425262728292a2b2c2d2e2f
303132333435363738393a3b3c3d3e3f
404142434445464748494a4b4c4d4e4f
505152535455565758595a5b5c5d5e5f
606162636465666768696a6b6c6d6e6f
707172737475767778797a7b7c7d7e7f
808182838485868788898a8b8c8d8e8f
909192939495969798999a9b9c9d9e9f
a0a1a2a3a4a5a6a7a8a9aaabacadaeaf
b0b1b2b3b4b5b6b7b8b9babbbcbdbebf
c0c1c2c3c4c5c6c7c8c9cacbcccdcecf
d0d1d2d3d4d5d6d7d8d9dadbdcdddedf
e0e1e2e3e4e5e6e7e8e9eaebecedeeef
f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff
===DONE===

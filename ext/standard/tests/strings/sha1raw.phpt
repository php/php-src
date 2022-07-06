--TEST--
sha1() with RAW output
--FILE--
<?php
echo bin2hex(sha1("abc", TRUE))."\n";
echo bin2hex(sha1("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", TRUE))."\n";
echo bin2hex(sha1("a", TRUE))."\n";
echo bin2hex(sha1("0123456701234567012345670123456701234567012345670123456701234567", TRUE))."\n";
?>
--EXPECT--
a9993e364706816aba3e25717850c26c9cd0d89d
84983e441c3bd26ebaae4aa1f95129e5e54670f1
86f7e437faa5a7fce15d1ddcb9eaeaea377667b8
e0c094e867ef46c350ef54a7f59dd60bed92ae83

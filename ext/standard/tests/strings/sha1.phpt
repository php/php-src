--TEST--
sha1() with ASCII output
--FILE--
<?php
echo sha1("abc")."\n";
echo sha1("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq")."\n";
echo sha1("a")."\n";
echo sha1("0123456701234567012345670123456701234567012345670123456701234567")."\n";
?>
--EXPECT--
a9993e364706816aba3e25717850c26c9cd0d89d
84983e441c3bd26ebaae4aa1f95129e5e54670f1
86f7e437faa5a7fce15d1ddcb9eaeaea377667b8
e0c094e867ef46c350ef54a7f59dd60bed92ae83

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
34aa973cd4c4daa4f61eeb2bdbad27316534016f
dea356a2cddd90c7a7ecedc5ebb563934f460452

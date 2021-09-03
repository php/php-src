--TEST--
Hash: sha1 algorithm
--FILE--
<?php
echo hash('sha1', '') . "\n";
echo hash('sha1', 'a') . "\n";
echo hash('sha1', '012345678901234567890123456789012345678901234567890123456789') . "\n";

/* FIPS-180 Vectors */
echo hash('sha1', 'abc') . "\n";
echo hash('sha1', 'abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq') . "\n";
echo hash('sha1', str_repeat('a', 1000000)) . "\n";
?>
--EXPECT--
da39a3ee5e6b4b0d3255bfef95601890afd80709
86f7e437faa5a7fce15d1ddcb9eaeaea377667b8
f52e3c2732de7bea28f216d877d78dae1aa1ac6a
a9993e364706816aba3e25717850c26c9cd0d89d
84983e441c3bd26ebaae4aa1f95129e5e54670f1
34aa973cd4c4daa4f61eeb2bdbad27316534016f

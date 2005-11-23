--TEST--
ripemd160 algorithm
--SKIPIF--
<?php if(!extension_loaded("hash")) print "skip"; ?>
--FILE--
<?php
echo hash('ripemd160', '') . "\n";
echo hash('ripemd160', 'a') . "\n";
echo hash('ripemd160', 'abc') . "\n";
echo hash('ripemd160', 'message digest') . "\n";
echo hash('ripemd160', 'abcdefghijklmnopqrstuvwxyz') . "\n";
echo hash('ripemd160', 'abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq') . "\n";
echo hash('ripemd160', 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789') . "\n";
echo hash('ripemd160', '12345678901234567890123456789012345678901234567890123456789012345678901234567890') . "\n";
echo hash('ripemd160', str_repeat('a', 1000000)) . "\n";
--EXPECT--
9c1185a5c5e9fc54612808977ee8f548b2258d31
0bdc9d2d256b3ee9daae347be6f4dc835a467ffe
8eb208f7e05d987a9b044a8e98c6b087f15a0bfc
5d0689ef49d2fae572b881b123a85ffa21595f36
f71c27109c692c1b56bbdceb5b9d2865b3708dbc
12a053384a9c0c88e405a06c27dcf49ada62eb2b
b0e20b6e3116640286ed3a87a5713079b21f5189
9b752e45573d4b39f4dbd3323cab82bf63326bfb
52783243c1697bdbe16d37f97f68f08325dc1528

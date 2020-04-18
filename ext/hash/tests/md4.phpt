--TEST--
Hash: md4 algorithm
--FILE--
<?php
/* RFC 1320 vectors */
echo hash('md4', '') . "\n";
echo hash('md4', 'a') . "\n";
echo hash('md4', 'abc') . "\n";
echo hash('md4', 'message digest') . "\n";
echo hash('md4', 'abcdefghijklmnopqrstuvwxyz') . "\n";
echo hash('md4', 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789') . "\n";
echo hash('md4', '12345678901234567890123456789012345678901234567890123456789012345678901234567890') . "\n";
--EXPECT--
31d6cfe0d16ae931b73c59d7e0c089c0
bde52cb31de33e46245e05fbdbd6fb24
a448017aaf21d8525fc10ae87aa6729d
d9130a8164549fe818874806e1c7014b
d79e1c308aa5bbcdeea8ed63df412da9
043f8582f241db351ce627e153e7f0e4
e33b4ddc9c38f2199c3e7b164fcc0536

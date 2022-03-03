--TEST--
Phar: copy-on-write test 24 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite24.phar.php
phar.readonly=0
--EXTENSIONS--
phar
zlib
--FILE_EXTERNAL--
files/write24.phar
--EXPECT--
bool(false)
bool(true)
ok

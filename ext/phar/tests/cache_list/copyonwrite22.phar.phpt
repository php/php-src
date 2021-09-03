--TEST--
Phar: copy-on-write test 22 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite22.phar.php
phar.readonly=0
--EXTENSIONS--
phar
zlib
--FILE_EXTERNAL--
files/write22.phar
--EXPECT--
bool(true)
bool(false)
ok

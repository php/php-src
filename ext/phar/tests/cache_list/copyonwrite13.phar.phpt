--TEST--
Phar: copy-on-write test 13 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite13.phar.php
phar.readonly=0
--EXTENSIONS--
phar
zlib
--FILE_EXTERNAL--
files/write13.phar
--EXPECT--
bool(false)
bool(true)
ok

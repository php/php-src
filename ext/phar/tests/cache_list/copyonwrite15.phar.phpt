--TEST--
Phar: copy-on-write test 15 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite15.phar.php
phar.readonly=0
--EXTENSIONS--
phar
--FILE_EXTERNAL--
files/write15.phar
--EXPECT--
bool(false)
bool(true)
ok

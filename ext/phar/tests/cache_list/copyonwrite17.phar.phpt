--TEST--
Phar: copy-on-write test 17 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite17.phar.php
phar.readonly=0
--EXTENSIONS--
phar
--FILE_EXTERNAL--
files/write17.phar
--EXPECT--
NULL
string(2) "hi"
ok

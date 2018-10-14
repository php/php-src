--TEST--
Phar: copy-on-write test 2 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite2.phar.php
phar.readonly=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE_EXTERNAL--
files/write2.phar
--EXPECT--
string(2) "hi"
bool(true)
string(2) "hi"
bool(true)
bool(true)
ok

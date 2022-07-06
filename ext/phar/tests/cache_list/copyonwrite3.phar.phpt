--TEST--
Phar: copy-on-write test 3 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite3.phar.php
phar.readonly=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE_EXTERNAL--
files/write3.phar
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
ok

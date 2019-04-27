--TEST--
Phar: copy-on-write test 4 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite4.phar.php
phar.readonly=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE_EXTERNAL--
files/write4.phar
--EXPECT--
bool(false)
bool(true)
string(2) "hi"
ok

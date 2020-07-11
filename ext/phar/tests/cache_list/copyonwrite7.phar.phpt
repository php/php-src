--TEST--
Phar: copy-on-write test 7 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite7.phar.php
phar.readonly=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE_EXTERNAL--
files/write7.phar
--EXPECT--
bool(true)
bool(false)
ok

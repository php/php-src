--TEST--
Phar: copy-on-write test 15 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite15.phar.php
phar.readonly=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE_EXTERNAL--
files/write15.phar
--EXPECTF--
bool(false)
bool(true)
ok

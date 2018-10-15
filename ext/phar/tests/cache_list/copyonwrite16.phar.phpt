--TEST--
Phar: copy-on-write test 16 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite16.phar.php
phar.readonly=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE_EXTERNAL--
files/write16.phar
--EXPECTF--
bool(true)
bool(false)
ok

--TEST--
Phar: copy-on-write test 20 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite20.phar.php
phar.readonly=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE_EXTERNAL--
files/write20.phar
--EXPECTF--
string(2) "hi"
NULL
ok

--TEST--
Phar: copy-on-write test 8 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite8.phar.php
phar.readonly=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE_EXTERNAL--
files/write8.phar
--EXPECTF--
string(%s) "%scopyonwrite8.phar.php"
hi
ok

--TEST--
Phar: copy-on-write test 25 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite25.phar.php
phar.readonly=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip no zlib extension"); ?>
--FILE_EXTERNAL--
files/write25.phar
--EXPECTF--
bool(false)
bool(true)
<?php __HALT_COMPILER();
ok

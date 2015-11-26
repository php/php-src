--TEST--
Phar: copy-on-write test 18 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite18.phar.php
phar.readonly=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE_EXTERNAL--
files/write18.phar
--EXPECTF--
100666
100444
ok
--TEST--
Phar: copy-on-write test 19 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite19.phar.php
phar.readonly=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE_EXTERNAL--
files/write19.phar
--EXPECTF--
string(2) "hi"
%string|unicode%(3) "hi2"
ok
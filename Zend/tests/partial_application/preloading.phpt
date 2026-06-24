--TEST--
PFA preloading
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.preload={PWD}/preloading.inc
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php

var_dump(test()(1));

?>
--EXPECT--
int(2)
int(2)

--TEST--
PFA preloading
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.preload={PWD}/preloading.inc
--FILE--
<?php

var_dump(test()(1));

?>
--EXPECT--
int(2)
int(2)

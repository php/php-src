--TEST--
Handling of auto globals during preloading
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_globals.inc
--EXTENSIONS--
opcache
--FILE--
<?php
$x = 123;
var_dump(get_x());
?>
--EXPECT--
int(123)

--TEST--
Nested function definition
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_nested_function.inc
--EXTENSIONS--
opcache
--FILE--
<?php
test();
test2();
?>
===DONE===
--EXPECT--
===DONE===

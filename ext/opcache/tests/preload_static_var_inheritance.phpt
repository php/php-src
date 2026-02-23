--TEST--
Bug #79548: Preloading segfault with inherited method using static variable
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_static_var_inheritance.inc
--EXTENSIONS--
opcache
--FILE--
<?php
var_dump((new B)->test());
?>
--EXPECT--
NULL

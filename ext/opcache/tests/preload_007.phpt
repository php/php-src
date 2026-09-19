--TEST--
Handling of includes that were not executed
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_include.inc
--EXTENSIONS--
opcache
--FILE--
<?php
echo "Foobar";
?>
--EXPECT--
Foobar

--TEST--
Preloading of the property info table with internal parent
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload.inc
--EXTENSIONS--
opcache
--FILE--
<?php
$e = new MyException("foo");
echo $e->getMessage(), "\n";
?>
--EXPECT--
foo

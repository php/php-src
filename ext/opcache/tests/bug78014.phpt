--TEST--
Bug #78014 (Preloaded classes may depend on non-preloaded classes due to unresolved consts)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_bug78014.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
$c = new C;
var_dump($c->foo());
?>
--EXPECT--
int(42)

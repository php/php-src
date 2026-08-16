--TEST--
Preloading of anonymous class
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
var_dump(get_anon());
?>
--EXPECTF--
object(class@anonymous%0%s:65$%x)#1 (0) {
}

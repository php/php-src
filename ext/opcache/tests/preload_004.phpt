--TEST--
Preloading class with undefined class constant access
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_undef_const.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
var_dump(class_exists('Foo'));
try {
   new Foo();
} catch (Throwable $ex) {
	echo $ex->getMessage() . "\n";
}
?>
--EXPECT--
bool(true)
Undefined constant self::DOES_NOT_EXIST

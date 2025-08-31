--TEST--
Preloading class using trait with undefined class constant access
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_undef_const_2.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
var_dump(trait_exists('T'));
var_dump(class_exists('Foo'));
try {
    new Foo();
} catch (Throwable $ex) {
    echo $ex->getMessage() . "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
Undefined constant "UNDEF"

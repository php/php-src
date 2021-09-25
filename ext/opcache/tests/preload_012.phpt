--TEST--
No autoloading during constant resolution
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_const_autoload.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
class Foo {
}
var_dump(class_exists('Test'));
try {
    new Test();
} catch (Throwable $ex) {
    echo $ex->getMessage() . "\n";
}
?>
--EXPECT--
bool(true)
Undefined constant Foo::BAR

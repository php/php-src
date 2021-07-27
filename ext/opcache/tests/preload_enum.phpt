--TEST--
Enum preloading
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_enum.inc
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
--FILE--
<?php

var_dump(MyEnum::Foo);
var_dump(MyEnum::Bar);

?>
--EXPECTF--
enum(MyEnum::Bar)

Warning: Can't preload unlinked class MyEnum2: Unknown reason in %s on line %d
enum(MyEnum::Foo)
enum(MyEnum::Bar)

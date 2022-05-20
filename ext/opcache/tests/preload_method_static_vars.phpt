--TEST--
Preloading inherited method with separated static vars
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_method_static_vars.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
if (PHP_ZTS) die('xfail GH-8588');
?>
--FILE--
<?php
Foo::test();
Bar::test();
?>
--EXPECT--
int(1)
int(2)
int(2)
int(3)

int(1)
int(1)

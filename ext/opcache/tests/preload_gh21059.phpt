--TEST--
GH-21059: Segfault when preloading constant AST closure
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.preload={PWD}/preload_gh21059.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
(Foo::C)();
?>
--EXPECT--
Hello world

--TEST--
GH-24701 (Use-after-free on typed trait constants during opcache preload)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_gh24701.inc
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
var_dump(C::x());
var_dump(C::Y);
?>
--EXPECT--
enum(E::A)
enum(F::B)

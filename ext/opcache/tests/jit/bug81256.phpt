--TEST--
Bug #81256: Assertion `zv != ((void *)0)' failed for "preload" with JIT
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY === "Windows") die("skip Windows does not support preloading");
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.preload={PWD}/preload_bug81256.inc
--FILE--
<?php
foo();
?>
--EXPECT--
Hello

--TEST--
Bug #81256: Assertion `zv != ((void *)0)' failed for "preload" with JIT
--EXTENSIONS--
opcache
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

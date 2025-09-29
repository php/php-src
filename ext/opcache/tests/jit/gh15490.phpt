--TEST--
GH-15490: use-after-free when traversing call graph
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.preload={PWD}/gh15490.inc
opcache.jit=1235
--FILE--
<?php
foo();
?>
--EXPECT--
Hello world!

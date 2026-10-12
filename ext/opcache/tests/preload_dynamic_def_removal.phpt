--TEST--
Preloading dynamic def in method/function
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_dynamic_def_removal.inc
--EXTENSIONS--
opcache
--FILE--
<?php
dynamic();
dynamic2();
dynamic_in_hook();
?>
--EXPECT--
dynamic
dynamic2
dynamic in hook

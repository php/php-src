--TEST--
Bug #78175 (Preloading segfaults at preload time and at runtime)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_bug78175.inc
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
OK
--EXPECT--
Shutdown
OK

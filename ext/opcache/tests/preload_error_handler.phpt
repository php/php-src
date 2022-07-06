--TEST--
Bug #81353: Segfault with preloading and error handler using static variables
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_error_handler.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
===DONE===
--EXPECTF--
Warning: Can't preload unlinked class B: Unknown parent A in %s on line %d
===DONE===

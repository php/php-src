--TEST--
Parse error in preload script
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_parse_error.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
if (getenv('SKIP_ASAN')) die('xleak Startup failure leak');
?>
--FILE--
OK
--EXPECTF--
Parse error: syntax error, unexpected identifier "error" in %s on line %d

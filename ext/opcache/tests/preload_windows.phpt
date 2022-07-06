--TEST--
Preloading is not supported on Windows
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY != 'Windows') die('skip Windows only test');
?>
--FILE--
Unreachable
--EXPECTF--
%s: Error Preloading is not supported on Windows

--TEST--
Preloading basic test
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
var_dump(function_exists("f1"));
var_dump(function_exists("f2"));

$rt = true;
include(__DIR__ . "/preload.inc");
var_dump(function_exists("f2"));
?>
OK
--EXPECT--
bool(true)
bool(false)
bool(true)
OK

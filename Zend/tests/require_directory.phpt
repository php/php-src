--TEST--
Including a directory generates an error
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die("skip Not for Windows");
?>
--FILE--
<?php

/* Just a random test directory */
$dir = __DIR__ . '/variadic';
require $dir;

?>
--EXPECTF--
Notice: require(): Read of %d bytes failed with errno=21 Is a directory in %s on line %d

Fatal error: Uncaught Error: Failed opening required '%s' (include_path='.:') in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d

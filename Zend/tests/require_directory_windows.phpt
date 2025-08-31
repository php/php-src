--TEST--
Including a directory generates an error (Windows variant)
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die("skip Only for Windows");
?>
--FILE--
<?php

/* Just a random test directory */
$dir = __DIR__ . '/variadic';
require $dir;

?>
--EXPECTF--
Warning: require(%s): Failed to open stream: Permission denied in %s on line %d

Fatal error: Uncaught Error: Failed opening required '%s' (include_path='%s') in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d

--TEST--
Including a directory generates an error
--FILE--
<?php

/* Just a random test directory */
$dir = __DIR__ . '/variadic';
require $dir;

?>
--EXPECTF--
Notice: require(): read of %d bytes failed with errno=21 Is a directory in %s on line %d

Fatal error: require(): Failed opening required '%s' (include_path='%s') in %s on line %d

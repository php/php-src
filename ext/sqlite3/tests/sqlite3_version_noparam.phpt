--TEST--
SQLite3::version test, testing for missing function parameters
--CREDITS--
Michelangelo van Dam
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

var_dump(SQLite3::version('dummy'));

?>
--EXPECTF--
Warning: SQLite3::version() expects exactly 0 parameters, 1 given in %s on line %d
NULL

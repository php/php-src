--TEST--
SQLite3::exec test, testing for wrong type parameters
--CREDITS--
Michelangelo van Dam
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

$db = new SQLite3(':memory:');
$db->exec(array ('a','b','c'), 20090509);

?>
--EXPECTF--
Warning: SQLite3::exec() expects exactly 1 parameter, 2 given in %s on line %d

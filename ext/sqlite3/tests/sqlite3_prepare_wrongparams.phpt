--TEST--
SQLite3::prepare test, testing for wrong parameters
--CREDITS--
Michelangelo van Dam
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE foo (id INTEGER, bar STRING)');
$db->exec("INSERT INTO foo (id, bar) VALUES (1, 'This is a test')");

$stmt = $db->prepare();

?>
--EXPECTF--
Warning: SQLite3::prepare() expects exactly 1 parameter, 0 given in %s on line %d

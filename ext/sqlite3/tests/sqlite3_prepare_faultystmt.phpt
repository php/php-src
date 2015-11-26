--TEST--
SQLite3::prepare test, testing for faulty statement
--CREDITS--
Michelangelo van Dam
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE foo (id INTEGER, bar STRING)');
$db->exec("INSERT INTO foo (id, bar) VALUES (1, 'This is a test')");

$stmt = $db->prepare('SELECT foo FROM bar');

var_dump($stmt);
?>
--EXPECTF--
Warning: SQLite3::prepare(): Unable to prepare statement: 1, no such table: bar in %s on line %d
bool(false)

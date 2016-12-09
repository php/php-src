--TEST--
SQLite3Result::reset test, testing an exception is raised when calling reset with parameters
--CREDITS--
Michelangelo van Dam
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE foo (bar STRING)');
$db->exec("INSERT INTO foo (bar) VALUES ('This is a test')");
$db->exec("INSERT INTO foo (bar) VALUES ('This is another test')");

$result = $db->query('SELECT bar FROM foo');
$result->reset(1);
?>
--EXPECTF--
Warning: SQLite3Result::reset() expects exactly 0 parameters, 1 given in %s on line %d

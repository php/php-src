--TEST--
SQLite3Result::fetchArray() test, testing two params causes a failure
--CREDITS--
Michelangelo van Dam
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE foo (bar STRING)');
$db->exec("INSERT INTO foo (bar) VALUES ('This is a test')");
$db->exec("INSERT INTO foo (bar) VALUES ('This is another test')");

$result = $db->query('SELECT bar FROM foo');
var_dump($result->fetchArray(1,2));
?>
--EXPECTF--
Warning: SQLite3Result::fetchArray() expects at most 1 parameter, 2 given in %s on line %d
NULL

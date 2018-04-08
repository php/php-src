--TEST--
SQLite3::query parameters
--CREDITS--
Jachim Coudenys
# TestFest 2009 Belgium
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

$db = new SQLite3(':memory:');
echo 'Testing SQLite3 query without parameters' . PHP_EOL;
$db->query();

echo 'Testing SQLite3 query with one array parameter' . PHP_EOL;
$db->query(array());

echo 'Testing SQLite3 qeury with empty string parameter' . PHP_EOL;
var_dump($db->query(''));

echo "Done";
?>
--EXPECTF--
Testing SQLite3 query without parameters

Warning: SQLite3::query() expects exactly 1 parameter, 0 given in %s on line %d
Testing SQLite3 query with one array parameter

Warning: SQLite3::query() expects parameter 1 to be string, array given in %s on line %d
Testing SQLite3 qeury with empty string parameter
bool(false)
Done

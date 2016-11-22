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
echo 'Testing SQLite3 querySingle without parameters' . PHP_EOL;
$db->querySingle();

echo 'Testing SQLite3 querySingle with one array parameter' . PHP_EOL;
$db->querySingle(array());

echo 'Testing SQLite3 qeurySingle with empty string parameter' . PHP_EOL;
var_dump($db->querySingle(''));

echo "Done";
?>
--EXPECTF--
Testing SQLite3 querySingle without parameters

Warning: SQLite3::querySingle() expects at least 1 parameter, 0 given in %s on line %d
Testing SQLite3 querySingle with one array parameter

Warning: SQLite3::querySingle() expects parameter 1 to be string, array given in %s on line %d
Testing SQLite3 qeurySingle with empty string parameter
bool(false)
Done

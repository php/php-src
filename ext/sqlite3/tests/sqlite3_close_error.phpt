--TEST--
SQLite3::close parameters
--CREDITS--
Jachim Coudenys
# TestFest 2009 Belgium
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

$db = new SQLite3(':memory:');
echo 'Testing SQLite3 close with one parameter' . PHP_EOL;
$db->close('parameter');

echo "Done";
?>
--EXPECTF--
Testing SQLite3 close with one parameter

Warning: SQLite3::close() expects exactly 0 parameters, 1 given in %s on line %d
Done

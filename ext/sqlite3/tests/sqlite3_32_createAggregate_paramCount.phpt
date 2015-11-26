--TEST--
SQLite3::createAggregate Test that an error is thrown when no parameters are present
--CREDITS--
James Cauwelier
# Belgium PHP TestFest
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

$db = new SQLite3(':memory:');

$db->createAggregate ();

$db->close();

echo "Done"
?>
--EXPECTF--
Warning: SQLite3::createAggregate() expects at least 3 parameters, 0 given in %s on line %d
Done

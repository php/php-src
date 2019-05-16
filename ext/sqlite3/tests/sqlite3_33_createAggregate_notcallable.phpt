--TEST--
SQLite3::createAggregate() Test whether a supplied PHP function is valid when using in an aggregate function
--CREDITS--
James Cauwelier
# Belgium PHP TestFest (2009)
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

function aggregate_step ($var) { return $var; }
function aggregate_final ($var) { return $var; }

$db = new SQLite3(':memory:');

$db->createAggregate ('TESTAGGREGATE', 'aggregate_test_step', 'aggregate_final');
$db->createAggregate ('TESTAGGREGATE2', 'aggregate_step', 'aggregate_test_final');
var_dump($db->createAggregate ('TESTAGGREGATE3', 'aggregate_step', 'aggregate_final'));

$db->close();

echo "Done"
?>
--EXPECTF--
Warning: SQLite3::createAggregate(): Not a valid callback function aggregate_test_step in %s on line %d

Warning: SQLite3::createAggregate(): Not a valid callback function aggregate_test_final in %s on line %d
bool(true)
Done

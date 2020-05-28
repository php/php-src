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

try {
    $db->createAggregate('TESTAGGREGATE', 'aggregate_test_step', 'aggregate_final');
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $db->createAggregate('TESTAGGREGATE2', 'aggregate_step', 'aggregate_test_final');
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump($db->createAggregate('TESTAGGREGATE3', 'aggregate_step', 'aggregate_final'));

$db->close();

echo "Done"
?>
--EXPECT--
SQLite3::createAggregate(): Argument #2 ($step_callback) must be of type callable, string given
SQLite3::createAggregate(): Argument #3 ($final_callback) must be of type callable, string given
bool(true)
Done

--TEST--
PDO_sqlite: Testing invalid callback for sqliteCreateAggregate()
--SKIPIF--
<?php if (!extension_loaded('pdo_sqlite')) print 'skip not loaded'; ?>
--FILE--
<?php

$pdo = new PDO('sqlite::memory:');

try {
    $pdo->sqliteCreateAggregate('foo', 'a', '');
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    $pdo->sqliteCreateAggregate('foo', 'strlen', '');
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
PDO::sqliteCreateAggregate(): Argument #2 ($step) must be a valid callback, function "a" not found or invalid function name
PDO::sqliteCreateAggregate(): Argument #3 ($finalize) must be a valid callback, function "" not found or invalid function name

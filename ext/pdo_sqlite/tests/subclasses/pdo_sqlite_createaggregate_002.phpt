--TEST--
PDO_sqlite: Testing invalid callback for createAggregate()
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

// This test was copied from the pdo_sqlite test for sqliteCreateAggregate
$pdo = new PdoSqlite('sqlite::memory:');

try {
    $pdo->createAggregate('foo', 'a', '');
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    $pdo->createAggregate('foo', 'strlen', '');
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
PdoSqlite::createAggregate(): Argument #2 ($step) must be a valid callback, function "a" not found or invalid function name
PdoSqlite::createAggregate(): Argument #3 ($finalize) must be a valid callback, function "" not found or invalid function name

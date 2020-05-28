--TEST--
PDO_sqlite: Testing invalid callback for sqliteCreateAggregate()
--SKIPIF--
<?php if (!extension_loaded('pdo_sqlite')) print 'skip not loaded'; ?>
--FILE--
<?php

$pdo = new PDO('sqlite::memory:');

try {
    $pdo->sqliteCreateAggregate('foo', 'a', '');
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $pdo->sqliteCreateAggregate('foo', 'strlen', '');
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
PDO::sqliteCreateAggregate(): Argument #2 must be of type callable, string given
PDO::sqliteCreateAggregate(): Argument #3 must be of type callable, string given

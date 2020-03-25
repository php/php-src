--TEST--
PDO_sqlite: Testing invalid callback for sqliteCreateAggregate()
--SKIPIF--
<?php if (!extension_loaded('pdo_sqlite')) print 'skip not loaded'; ?>
--FILE--
<?php

$pdo = new PDO('sqlite::memory:');

$pdo->sqliteCreateAggregate('foo', 'a', '');
$pdo->sqliteCreateAggregate('foo', 'strlen', '');

?>
--EXPECTF--
Warning: PDO::sqliteCreateAggregate(): Function 'a' is not callable in %s on line %d

Warning: PDO::sqliteCreateAggregate(): Function '' is not callable in %s on line %d

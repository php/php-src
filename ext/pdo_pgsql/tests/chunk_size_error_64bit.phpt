--TEST--
PDO PgSQL Pdo\Pgsql::ATTR_CHUNK_SIZE upper bound, 64-bit
--EXTENSIONS--
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
if (!defined('Pdo\Pgsql::ATTR_CHUNK_SIZE')) die('skip libpq >= 17 required');
if (PHP_INT_SIZE < 8) die('skip a value above INT_MAX needs 64-bit');
?>
--FILE--
<?php

require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

echo "=== INT_MAX is the largest chunk size ===\n";
$pdo->setAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE, 2147483647);
echo "accepted as: ";
var_dump($pdo->getAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE));

echo "=== one more is refused ===\n";
try {
    $pdo->setAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE, 2147483648);
} catch (ValueError $e) {
    echo 'setAttribute(): ', $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $pdo->prepare("SELECT 1", [Pdo\Pgsql::ATTR_CHUNK_SIZE => PHP_INT_MAX]);
} catch (ValueError $e) {
    echo 'prepare(): ', $e::class, ': ', $e->getMessage(), "\n";
}
echo "the attribute kept the accepted value: ";
var_dump($pdo->getAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE));
?>
--EXPECT--
=== INT_MAX is the largest chunk size ===
accepted as: int(2147483647)
=== one more is refused ===
setAttribute(): ValueError: Pdo\Pgsql::ATTR_CHUNK_SIZE must be between 0 and 2147483647
prepare(): ValueError: Pdo\Pgsql::ATTR_CHUNK_SIZE must be between 0 and 2147483647
the attribute kept the accepted value: int(2147483647)

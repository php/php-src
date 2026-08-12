--TEST--
PDO PgSQL a drained lazy fetch frees the connection without a prepared statement
--EXTENSIONS--
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php

require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

foreach ([
    'PDO::ATTR_EMULATE_PREPARES' => [PDO::ATTR_EMULATE_PREPARES => true],
    'Pdo\Pgsql::ATTR_DISABLE_PREPARES' => [Pdo\Pgsql::ATTR_DISABLE_PREPARES => true],
] as $label => $options) {
    $options[PDO::ATTR_PREFETCH] = 0;

    $stmt = $pdo->prepare("VALUES (1), (2)", $options);
    $stmt->execute();
    $stmt->fetchAll();

    $stmt = $pdo->prepare("VALUES (1), (2)", $options);
    $stmt->execute();
    echo "$label: ";
    var_dump((bool) $stmt->fetchAll());
}
?>
--EXPECT--
PDO::ATTR_EMULATE_PREPARES: bool(true)
Pdo\Pgsql::ATTR_DISABLE_PREPARES: bool(true)

--TEST--
GH-21869 pdo_pgsql: DEALLOCATE failure in destructor must not poison the enclosing transaction
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
$pdo = PDOTest::factory();
if (version_compare($pdo->getAttribute(PDO::ATTR_CLIENT_VERSION), '17', '>=')) {
    die('skip libpq >= 17 uses PQclosePrepared instead of the DEALLOCATE query');
}
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

$pdo->exec('CREATE TABLE gh21869 (a integer not null)');

$pdo->beginTransaction();
$stmt = $pdo->prepare('INSERT INTO gh21869 (a) VALUES (?)');
$stmt->execute([1]);

foreach ($pdo->query('SELECT name FROM pg_prepared_statements') as $row) {
    $pdo->exec('DEALLOCATE ' . $row['name']);
}

unset($stmt);

$pdo->commit();

var_dump((int) $pdo->query('SELECT count(*) FROM gh21869')->fetchColumn());
?>
--CLEAN--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->exec('DROP TABLE IF EXISTS gh21869');
?>
--EXPECT--
int(1)

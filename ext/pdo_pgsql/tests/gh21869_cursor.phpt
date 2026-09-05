--TEST--
GH-21869 pdo_pgsql: closing a scrollable cursor must not poison the enclosing transaction
--EXTENSIONS--
pdo
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

$pdo->exec('CREATE TABLE gh21869_cursor (a integer not null)');

/* a rollback drops the cursor, so the destructor's CLOSE no longer matches anything */
$pdo->beginTransaction();
$stmt = $pdo->prepare('SELECT a FROM gh21869_cursor', [PDO::ATTR_CURSOR => PDO::CURSOR_SCROLL]);
$stmt->execute();
$pdo->rollBack();

$pdo->beginTransaction();
unset($stmt);
$pdo->exec('INSERT INTO gh21869_cursor (a) VALUES (1)');
$pdo->commit();

var_dump((int) $pdo->query('SELECT count(*) FROM gh21869_cursor')->fetchColumn());

/* the cursor was never declared, there is nothing to close */
$pdo->beginTransaction();
$stmt = $pdo->prepare('SELECT a FROM gh21869_cursor', [PDO::ATTR_CURSOR => PDO::CURSOR_SCROLL]);
unset($stmt);
$pdo->exec('INSERT INTO gh21869_cursor (a) VALUES (2)');
$pdo->commit();

var_dump((int) $pdo->query('SELECT count(*) FROM gh21869_cursor')->fetchColumn());
?>
--CLEAN--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->exec('DROP TABLE IF EXISTS gh21869_cursor');
?>
--EXPECT--
int(1)
int(2)

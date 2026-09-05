--TEST--
GH-21869 pdo_pgsql: a cursor CLOSE held back during a transaction runs once it ends
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

function open_cursors(PDO $pdo): int
{
    return (int) $pdo->query("SELECT count(*) FROM pg_cursors WHERE name LIKE 'pdo_crsr%'")->fetchColumn();
}

$pdo->exec('CREATE TABLE gh21869_cursor_deferred (a integer not null)');

/* declared outside a transaction, so WITH HOLD keeps it alive */
$stmt = $pdo->prepare('SELECT a FROM gh21869_cursor_deferred', [PDO::ATTR_CURSOR => PDO::CURSOR_SCROLL]);
$stmt->execute();

/* an unrelated rollback: the cursor can no longer be assumed to exist */
$pdo->beginTransaction();
$pdo->rollBack();

$pdo->beginTransaction();
unset($stmt);
var_dump(open_cursors($pdo));
$pdo->commit();
var_dump(open_cursors($pdo));
?>
--CLEAN--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->exec('DROP TABLE IF EXISTS gh21869_cursor_deferred');
?>
--EXPECT--
int(1)
int(0)

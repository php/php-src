--TEST--
PDO PgSQL PDO::CURSOR_SCROLL keeps track of a held cursor when the CLOSE before a re-declare fails
--EXTENSIONS--
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require dirname(__DIR__, 2) . '/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php

require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$stmt = $db->prepare('SELECT CAST(:v AS int)', [PDO::ATTR_CURSOR => PDO::CURSOR_SCROLL]);
$stmt->execute([':v' => '1']);

$db->beginTransaction();

try {
    $db->exec('SELECT 1 / 0');
} catch (PDOException $e) {
    echo $e::class, ': ', $e->getCode(), PHP_EOL;
}

try {
    $stmt->execute([':v' => '2']);
} catch (PDOException $e) {
    echo $e::class, ': ', $e->getCode(), PHP_EOL;
}

$db->rollBack();
unset($stmt);

var_dump($db->query("SELECT count(*) FROM pg_cursors WHERE name LIKE 'pdo\_crsr\_%'")->fetchColumn());

?>
--EXPECT--
PDOException: 22012
PDOException: 25P02
string(1) "0"

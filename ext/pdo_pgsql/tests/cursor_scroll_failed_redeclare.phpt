--TEST--
PDO PgSQL PDO::CURSOR_SCROLL sends no CLOSE after a failed re-declare
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

try {
    $stmt->execute([':v' => 'not an int']);
} catch (PDOException $e) {
    echo $e::class, ': ', $e->getCode(), PHP_EOL;
}

$db->beginTransaction();
unset($stmt);

$db->exec('SELECT 2');

echo 'Done', PHP_EOL;

?>
--EXPECT--
PDOException: 22P02
Done

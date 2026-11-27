--TEST--
GH-25838 (pdo_pgsql: use-after-free when a lazy statement follows a failed query)
--EXTENSIONS--
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--CREDITS--
Yun Dou
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

try {
    $pdo->query('SELECT 1 / 0');
} catch (PDOException $e) {
    echo $e->getCode(), PHP_EOL;
}

$pdo->setAttribute(PDO::ATTR_PREFETCH, 0);
echo $pdo->query('SELECT 43')->fetchColumn(), PHP_EOL;
?>
--EXPECT--
22012
43

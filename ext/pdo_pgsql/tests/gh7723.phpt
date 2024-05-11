--TEST--
GitHub #7723 (Fix error message allocation of PDO PgSQL)
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
require __DIR__ . '/config.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
require __DIR__ . '/config.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_PERSISTENT, true);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

$st = $db->prepare('select 1');
for ($i = 0; ++$i <= 2;) {
    try {
        $st->bindValue(':invalid', $i);
    } catch (PDOException $e) {
        echo $e->getMessage() . "\n";
    }
}
?>
--EXPECT--
SQLSTATE[HY093]: Invalid parameter number: :invalid
SQLSTATE[HY093]: Invalid parameter number: :invalid

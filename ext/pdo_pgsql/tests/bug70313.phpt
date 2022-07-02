--TEST--
PDO PgSQL Bug #70313 (PDO statement fails to throw exception)
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
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
try {
    $stmt = $db->prepare(");");

    $stmt->execute([1]);
} catch (PDOException $e) {
    echo $e->getMessage(), "\n";
}

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
try {
    $stmt = $db->prepare(");");

    $stmt->execute([1]);
} catch (PDOException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
SQLSTATE[42601]: Syntax error: %A
SQLSTATE[HY093]: Invalid parameter number: number of bound variables does not match number of tokens

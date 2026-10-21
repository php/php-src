--TEST--
GH-24490 (pdo_pgsql: prepare() hangs on a query containing a NUL byte)
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

foreach ([true, false] as $emulate) {
    $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, $emulate);
    $db->prepare("SELECT 'abc\0def'");
    $db->prepare("\0SELECT 1");
    try {
        $db->prepare("SELECT '\0', ?, :p");
    } catch (\Throwable $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
}
echo "Done\n";
?>
--EXPECT--
PDOException: SQLSTATE[HY093]: Invalid parameter number: mixed named and positional parameters
PDOException: SQLSTATE[HY093]: Invalid parameter number: mixed named and positional parameters
Done

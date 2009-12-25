--TEST--
Bug #49985 (pdo_pgsql prepare() re-use previous aborted transaction)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$db->exec("CREATE TABLE test (a int PRIMARY KEY)");

for ($i = 0; $i < 3; $i++) {
    try {
        $db->beginTransaction();
        $stmt = $db->prepare("INSERT INTO test (a) VALUES (?)");
        var_dump($stmt->execute(array(1)));
        $db->commit();
    } catch (Exception $e) {
        echo trim(current(explode("\n", $e->getMessage())))."\n";
        $db->rollback();
    }
}

?>
--EXPECTF--
bool(true)
SQLSTATE[23505]: %s"test_pkey"
SQLSTATE[23505]: %s"test_pkey"


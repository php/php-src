--TEST--
Bug #49985 (pdo_pgsql prepare() re-use previous aborted transaction)
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

$db->exec("CREATE TABLE b49985 (a int PRIMARY KEY)");

for ($i = 0; $i < 3; $i++) {
    try {
        $db->beginTransaction();
        $stmt = $db->prepare("INSERT INTO b49985 (a) VALUES (?)");
        var_dump($stmt->execute(array(1)));
        $db->commit();
    } catch (Exception $e) {
        echo trim(current(explode("\n", $e->getMessage())))."\n";
        $db->rollback();
    }
}

?>
--CLEAN--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->query('DROP TABLE IF EXISTS b49985 CASCADE');
?>
--EXPECTF--
bool(true)
SQLSTATE[23505]: %sb49985_pkey%s
SQLSTATE[23505]: %sb49985_pkey%s

--TEST--
Bug #81037 PDO discards error message text from prepared statement
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$pdo = MySQLPDOTest::factory();
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
MySQLPDOTest::createTestTable($pdo);

$sql = "SELECT id FROM test WHERE label = :par";
$stmt = $pdo->prepare($sql);
try {
    $stmt->execute();
} catch (PDOException $e) {
    echo $e->getMessage(), "\n";
}
$data = $stmt->fetchAll(PDO::FETCH_ASSOC);

?>
--CLEAN--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::dropTestTable();
?>
--EXPECT--
SQLSTATE[HY093]: Invalid parameter number

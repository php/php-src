--TEST--
PDO MySQL Bug #75177 Type 'bit' is fetched as unexpected string
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
if (!MySQLPDOTest::isPDOMySQLnd()) die('skip only for mysqlnd');
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$pdo = MySQLPDOTest::factory();

$pdo->query("CREATE TABLE test_75177 (`bit` bit(8)) ENGINE=InnoDB");
$pdo->query("INSERT INTO test_75177 (`bit`) VALUES (1), (0b011), (0b01100)");

$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
$ret = $pdo->query("SELECT * FROM test_75177")->fetchAll();
foreach ($ret as $i) {
    var_dump($i["bit"]);
}

$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
$ret = $pdo->query("SELECT * FROM test_75177")->fetchAll();
foreach ($ret as $i) {
    var_dump($i["bit"]);
}

?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable(NULL, 'test_75177');
?>
--EXPECT--
int(1)
int(3)
int(12)
int(1)
int(3)
int(12)

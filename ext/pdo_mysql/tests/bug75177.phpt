--TEST--
PDO MySQL Bug #75177 Type 'bit' is fetched as unexpected string
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
MySQLPDOTest::skipNotMySQLnd();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$pdo = MySQLPDOTest::factory();

$tbl = "test";
$pdo->query("DROP TABLE IF EXISTS $tbl");
$pdo->query("CREATE TABLE $tbl (`bit` bit(8)) ENGINE=InnoDB");
$pdo->query("INSERT INTO $tbl (`bit`) VALUES (1)");
$pdo->query("INSERT INTO $tbl (`bit`) VALUES (0b011)");
$pdo->query("INSERT INTO $tbl (`bit`) VALUES (0b01100)");

$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
$ret = $pdo->query("SELECT * FROM $tbl")->fetchAll();
foreach ($ret as $i) {
    var_dump($i["bit"]);
}

$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
$ret = $pdo->query("SELECT * FROM $tbl")->fetchAll();
foreach ($ret as $i) {
    var_dump($i["bit"]);
}

?>
--CLEAN--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::dropTestTable();
?>
--EXPECT--
int(1)
int(3)
int(12)
int(1)
int(3)
int(12)

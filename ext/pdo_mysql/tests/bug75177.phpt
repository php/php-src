--TEST--
PDO MySQL Bug #75177 Type 'bit' is fetched as unexpected string
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$pdo = MySQLPDOTest::factory();

$tbl = "tbl_bug75177";
$pdo->query("DROP TABLE IF EXISTS $tbl");
$pdo->query("CREATE TABLE $tbl (`bit` bit(8)) ENGINE=InnoDB");
$pdo->query("INSERT INTO $tbl (`bit`) VALUES (1)");
$pdo->query("INSERT INTO $tbl (`bit`) VALUES (0b011)");
$pdo->query("INSERT INTO $tbl (`bit`) VALUES (0b01100)");

$ret = $pdo->query("SELECT * FROM $tbl")->fetchAll();

foreach ($ret as $i) {
    var_dump($i["bit"]);
}

?>
--EXPECT--
string(1) "1"
string(1) "3"
string(2) "12"

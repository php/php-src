--TEST--
Bug #76815: PDOStatement cannot be GCed/closeCursor-ed when a PROCEDURE resultset SIGNAL
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$pdo = MySQLPDOTest::factory();
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$pdo->query('DROP FUNCTION IF EXISTS tst');
$pdo->query('DROP PROCEDURE IF EXISTS tst2');
$pdo->query('CREATE FUNCTION tst() RETURNS VARCHAR(5) DETERMINISTIC BEGIN RETURN \'x12345\'; END');
$pdo->query('CREATE PROCEDURE tst2() BEGIN SELECT tst(); END');

$st = $pdo->prepare('CALL tst2()');
try {
    $st->execute();
} catch (PDOException $ex) {
    echo $ex->getMessage(), "\n";
}
unset($st);
echo "Ok.\n";

?>
--CLEAN--
<?php
require_once __DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc';
$pdo = MySQLPDOTest::factory();
$pdo->query('DROP FUNCTION IF EXISTS tst');
$pdo->query('DROP PROCEDURE IF EXISTS tst2');
?>
--EXPECTF--
SQLSTATE[22001]: String data, right truncated: 1406 Data too long for column 'tst()' at row %d
Ok.

--TEST--
PDO PgSQL isInTransaction
--SKIPIF--
<?php # vim:se ft=php:
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
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

$db->exec('CREATE TABLE test (a integer not null primary key, b text)');

$db->beginTransaction();
try {
echo "Test PDO::PGSQL_TRANSACTION_INTRANS\n";
var_dump($db->inTransaction());

$stmt = $db->prepare("INSERT INTO test (a, b) values (?, ?)");
$stmt->bindValue(1, 1);
$stmt->bindValue(2, "test insert");
$stmt->execute();

$db->commit();

echo "Test PDO::PGSQL_TRANSACTION_IDLE\n";
var_dump($db->inTransaction());

$db->beginTransaction();

try {
$stmt = $db->prepare("INSERT INTO test (a, b) values (?, ?)");
$stmt->bindValue(1, "error");
$stmt->bindValue(2, "test insert");
$stmt->execute();
} catch (Exception $e) {
	/* We catch the exception because the execute will give error and we must test the PDO::PGSQL_TRANSACTION_ERROR */
	echo "Test PDO::PGSQL_TRANSACTION_INERROR\n";
	var_dump($db->inTransaction());
	$db->rollBack();
}

echo "Test PDO::PGSQL_TRANSACTION_IDLE\n";
var_dump($db->inTransaction());

} catch (Exception $e) {
	/* catch exceptions so that we can show the relative error */
	echo "Exception! at line ", $e->getLine(), "\n";
	var_dump($e->getMessage());
}

?>
--EXPECT--
Test PDO::PGSQL_TRANSACTION_INTRANS
bool(true)
Test PDO::PGSQL_TRANSACTION_IDLE
bool(false)
Test PDO::PGSQL_TRANSACTION_INERROR
bool(true)
Test PDO::PGSQL_TRANSACTION_IDLE
bool(false)

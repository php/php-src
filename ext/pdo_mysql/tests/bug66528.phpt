--TEST--
Bug #66528: No PDOException or errorCode if database becomes unavailable before PDO::commit
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$dbh = MySQLPDOTest::factory();
$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$dbh->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, false);

$dbh->exec('DROP TABLE IF EXISTS test');
$dbh->exec('CREATE TABLE test (a int) engine=innodb');
$dbh->beginTransaction();
$dbh->exec('INSERT INTO test (a) VALUES (1), (2)');
$stmt = $dbh->query('SELECT * FROM test');

try {
	$dbh->commit();
} catch (PDOException $e) {
	echo $e->getMessage(), "\n";
}

try {
	$dbh->rollBack();
} catch (PDOException $e) {
	echo $e->getMessage(), "\n";
}

try {
    $dbh->setAttribute(PDO::ATTR_AUTOCOMMIT, false);
} catch (PDOException $e) {
	echo $e->getMessage(), "\n";
}

?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECT--
SQLSTATE[HY000]: General error: 2014 Cannot execute queries while other unbuffered queries are active.  Consider using PDOStatement::fetchAll().  Alternatively, if your code is only ever going to run against mysql, you may enable query buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute.
SQLSTATE[HY000]: General error: 2014 Cannot execute queries while other unbuffered queries are active.  Consider using PDOStatement::fetchAll().  Alternatively, if your code is only ever going to run against mysql, you may enable query buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute.
SQLSTATE[HY000]: General error: 2014 Cannot execute queries while other unbuffered queries are active.  Consider using PDOStatement::fetchAll().  Alternatively, if your code is only ever going to run against mysql, you may enable query buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute.

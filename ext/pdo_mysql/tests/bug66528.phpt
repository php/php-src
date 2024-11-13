--TEST--
Bug #66528: No PDOException or errorCode if database becomes unavailable before PDO::commit
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$dbh = MySQLPDOTest::factory();

$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$dbh->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, false);

$dbh->exec('CREATE TABLE test_66528 (a INT) ENGINE=InnoDB');
$dbh->beginTransaction();
$dbh->exec('INSERT INTO test_66528 (a) VALUES (1), (2)');
$stmt = $dbh->query('SELECT * FROM test_66528');

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
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test_66528');
?>
--EXPECT--
SQLSTATE[HY000]: General error: 2014 Cannot execute queries while other unbuffered queries are active.  Consider using PDOStatement::fetchAll().  Alternatively, if your code is only ever going to run against mysql, you may enable query buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute.
SQLSTATE[HY000]: General error: 2014 Cannot execute queries while other unbuffered queries are active.  Consider using PDOStatement::fetchAll().  Alternatively, if your code is only ever going to run against mysql, you may enable query buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute.
SQLSTATE[HY000]: General error: 2014 Cannot execute queries while other unbuffered queries are active.  Consider using PDOStatement::fetchAll().  Alternatively, if your code is only ever going to run against mysql, you may enable query buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute.

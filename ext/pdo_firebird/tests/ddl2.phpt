--TEST--
PDO_Firebird: DDL/transactions 2
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php
require("testdb.inc");

$dbh = getDbConnection();
$dbh->exec("CREATE TABLE test_ddl2 (val int)");

$dbh->beginTransaction();
$dbh->exec("INSERT INTO test_ddl2 (val) VALUES (120)");
$dbh->exec("CREATE TABLE test_ddl2_2 (val INT)");
$dbh->rollback();

$result = $dbh->query("SELECT * FROM test_ddl2");
var_dump($result->fetchAll());

unset($dbh);
echo "done\n";
?>
--CLEAN--
<?php
require("testdb.inc");
$dbh = getDbConnection();
$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
@$dbh->exec('DROP TABLE test_ddl2');
@$dbh->exec('DROP TABLE test_ddl2_2');
?>
--EXPECT--
array(0) {
}
done

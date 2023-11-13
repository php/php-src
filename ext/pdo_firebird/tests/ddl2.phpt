--TEST--
PDO_Firebird: DDL/transactions 2
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php
require("testdb.inc");

$dbh->exec("CREATE TABLE test_ddl2 (val int)");

$dbh->beginTransaction();
$dbh->exec("INSERT INTO test_ddl2 (val) VALUES (120)");
$dbh->exec("CREATE TABLE test_ddl2_2 (val INT)");
$dbh->rollback();

$result = $dbh->query("SELECT * FROM test_ddl2");
foreach ($result as $r) {
    var_dump($r);
}

unset($dbh);
echo "done\n";
?>
--CLEAN--
<?php
require("testdb.inc");
$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
@$dbh->exec('DROP TABLE test_ddl2');
@$dbh->exec('DROP TABLE test_ddl2_2');
?>
--EXPECT--
done

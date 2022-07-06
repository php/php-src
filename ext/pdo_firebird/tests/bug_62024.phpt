--TEST--
Bug #62024 Cannot insert second row with null using parametrized query (Firebird PDO)
--SKIPIF--
<?php require('skipif.inc'); ?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php

require("testdb.inc");

$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
@$dbh->exec('DROP TABLE test_insert');
$dbh->exec("CREATE TABLE test_insert (ID INTEGER NOT NULL, TEXT VARCHAR(10))");

$dbh->commit();

//start actual test

$sql = "insert into test_insert (id, text) values (?, ?)";
$sttmt = $dbh->prepare($sql);

$args_ok = array(1, "test1");
$args_err = array(2, null);

$res = $sttmt->execute($args_ok);
var_dump($res);

$res = $sttmt->execute($args_err);
var_dump($res);

$dbh->commit();


//teardown test data
$sttmt = $dbh->prepare('DELETE FROM test_insert');
$sttmt->execute();

$dbh->commit();

$dbh->exec('DROP TABLE test_insert');

unset($sttmt);
unset($dbh);

?>
--EXPECT--
bool(true)
bool(true)

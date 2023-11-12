--TEST--
Bug #62024 Cannot insert second row with null using parametrized query (Firebird PDO)
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php

require("testdb.inc");

$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
$dbh->exec("CREATE TABLE test62024 (ID INTEGER NOT NULL, TEXT VARCHAR(10))");

//start actual test

$sql = "insert into test62024 (id, text) values (?, ?)";
$sttmt = $dbh->prepare($sql);

$args_ok = array(1, "test1");
$args_err = array(2, null);

$res = $sttmt->execute($args_ok);
var_dump($res);

$res = $sttmt->execute($args_err);
var_dump($res);


//teardown test data
$sttmt = $dbh->prepare('DELETE FROM test62024');
$sttmt->execute();

unset($sttmt);
unset($dbh);

?>
--CLEAN--
<?php
require 'testdb.inc';
@$dbh->exec("DROP TABLE test62024");
unset($dbh);
?>
--EXPECT--
bool(true)
bool(true)

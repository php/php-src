--TEST--
PDO_Firebird: error handle
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
$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

$table = 'error_handle';
$dbh->query("CREATE TABLE {$table} (val int)");

echo "dbh error";
$dbh->query("INSERT INTO {$table} VALUES ('str')");

echo "\n";

echo "stmt error";
$stmt = $dbh->prepare("INSERT INTO {$table} VALUES ('str')");
$stmt->execute();

unset($dbh);
?>
--CLEAN--
<?php
require 'testdb.inc';
@$dbh->exec('DROP TABLE error_handle');
unset($dbh);
?>
--EXPECTF--
dbh error
Warning: PDO::query(): SQLSTATE[22018]: Invalid character value for cast specification: -413 conversion error from string "str" in %s on line 10

stmt error
Warning: PDOStatement::execute(): SQLSTATE[22018]: Invalid character value for cast specification: -413 conversion error from string "str" in %s on line 16

--TEST--
PDO_Firebird: cursor should not be marked as opened on singleton statements
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php
require 'testdb.inc';

$dbh = getDbConnection();
$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
$dbh->exec('create table test_aaa (id integer)');
$S = $dbh->prepare('insert into test_aaa (id) values (:id) returning id');
$S->execute(['id' => 1]);
$S->execute(['id' => 2]);
unset($S);
unset($dbh);
echo 'OK';
?>
--CLEAN--
<?php
require 'testdb.inc';
$dbh = getDbConnection();
@$dbh->exec("DROP TABLE test_aaa");
unset($dbh);
?>
--EXPECT--
OK

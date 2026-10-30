--TEST--
PDO_Firebird: Bug 72931 Insert returning fails on Firebird 3
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
$dbh->exec('recreate table test72931 (id integer)');
$S = $dbh->prepare('insert into test72931 (id) values (1) returning id');
$S->execute();
$D = $S->fetch(PDO::FETCH_NUM);
echo $D[0];
unset($S);
unset($dbh);
?>
--CLEAN--
<?php
require 'testdb.inc';
$dbh = getDbConnection();
@$dbh->exec("DROP TABLE test72931");
unset($dbh);
?>
--EXPECT--
1

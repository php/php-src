--TEST--
PDO_Firebird: Bug 72931 Insert returning fails on Firebird 3
--SKIPIF--
<?php require('skipif.inc'); ?>
--FILE--
<?php
require 'testdb.inc';

@$dbh->exec('drop table tablea');
$dbh->exec('create table tablea (id integer)');
$S = $dbh->prepare('insert into tablea (id) values (1) returning id');
$S->execute();
$D = $S->fetch(PDO::FETCH_NUM);
echo $D[0];
unset($S);
unset($dbh);
?>
--EXPECT--
1

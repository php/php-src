--TEST--
PDO_Firebird: Bug 72931 Insert returning fails on Firebird 3
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php
require 'testdb.inc';

$dbh->exec('recreate table tablea (id integer)');
$S = $dbh->prepare('insert into tablea (id) values (1) returning id');
$S->execute();
$D = $S->fetch(PDO::FETCH_NUM);
echo $D[0];
unset($S);
unset($dbh);
?>
--EXPECT--
1

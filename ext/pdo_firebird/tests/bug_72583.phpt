--TEST--
PDO_Firebird: Feature 72583 Fetch integers as php integers not as strings
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php
require 'testdb.inc';

$dbh->exec('recreate table atable (aint integer, asmi smallint)');
$dbh->exec('insert into atable values (1, -1)');
$S = $dbh->prepare('select aint, asmi from atable');
$S->execute();
$D = $S->fetch(PDO::FETCH_NUM);
echo gettype($D[0])."\n".gettype($D[1]);
unset($S);
unset($dbh);
?>
--EXPECT--
integer
integer

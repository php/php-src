--TEST--
PDO_Firebird: Feature 72583 Fetch integers as php integers not as strings
--SKIPIF--
<?php if (!extension_loaded('interbase') || !extension_loaded('pdo_firebird')) die('skip'); ?>
--FILE--
<?php
require 'testdb.inc';
$C = new PDO('firebird:dbname='.$test_base, $user, $password) or die;
@$C->exec('drop table atable');
$C->exec('create table atable (aint integer, asmi smallint)');
$C->exec('insert into atable values (1, -1)');
$S = $C->prepare('select aint, asmi from atable');
$S->execute();
$D = $S->fetch(PDO::FETCH_NUM);
echo gettype($D[0])."\n".gettype($D[1]);
unset($S);
unset($C);
?>
--EXPECT--
integer
integer

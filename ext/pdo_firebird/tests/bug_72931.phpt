--TEST--
PDO_Firebird: Bug 72931 Insert returning fails on Firebird 3
--SKIPIF--
<?php if (!extension_loaded('interbase') || !extension_loaded('pdo_firebird')) die('skip'); ?>
--FILE--
<?php
require 'testdb.inc';
$C = new PDO('firebird:dbname='.$test_base, $user, $password) or die;
$C->exec('create table tablea (id integer)');
$S = $C->prepare('insert into tablea (id) values (1) returning id');
$S->execute();
$D = $S->fetch(PDO::FETCH_NUM);
echo $D[0];
unset($S);
unset($C);
?>
--CLEAN--
<?php
require 'testdb.inc';
$C = new PDO('firebird:dbname='.$test_base, $user, $password) or die;
$C->exec('drop table tablea');
?>
--EXPECT--
1

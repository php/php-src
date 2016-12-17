--TEST--
PDO_Firebird: cursor should not be marked as opened on singleton statements
--SKIPIF--
<?php if (!extension_loaded('interbase') || !extension_loaded('pdo_firebird')) die('skip'); ?>
--FILE--
<?php
require 'testdb.inc';
$C = new PDO('firebird:dbname='.$test_base, $user, $password, [PDO::ATTR_ERRMODE => PDO::ERRMODE_WARNING]) or die;
@$C->exec('drop table ta_table');
$C->exec('create table ta_table (id integer)');
$S = $C->prepare('insert into ta_table (id) values (:id) returning id');
$S->execute(['id' => 1]);
$S->execute(['id' => 2]);
unset($S);
unset($C);
echo 'OK';
?>
--EXPECT--
OK
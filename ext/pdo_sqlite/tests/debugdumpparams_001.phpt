--TEST--
Testing PDOStatement::debugDumpParams() with bound params
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) print 'skip not loaded';
?>
--FILE--
<?php

$db = new pdo('sqlite:memory');

$x= $db->prepare('select :a, :b');
$x->bindValue(':a', 1, PDO::PARAM_INT);
$x->bindValue(':b', 'foo');
var_dump($x->debugDumpParams());

?>
--EXPECT--
SQL: [13] select :a, :b
Params:  2
Key: Position #0:
paramno=-1
name=[2] :a
is_param=1
param_type=1
Key: Position #0:
paramno=-1
name=[2] :b
is_param=1
param_type=2
NULL

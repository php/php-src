--TEST--
Testing PDOStatement::debugDumpParams() with bound params
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new PDO('sqlite::memory:');

$x= $db->prepare('select :a, :b, ?');
$x->bindValue(':a', 1, PDO::PARAM_INT);
$x->bindValue(':b', 'foo');
$x->bindValue(3, 1313);
var_dump($x->debugDumpParams());

?>
--EXPECT--
SQL: [16] select :a, :b, ?
Params:  3
Key: Name: [2] :a
paramno=-1
name=[2] ":a"
is_param=1
param_type=1
Key: Name: [2] :b
paramno=-1
name=[2] ":b"
is_param=1
param_type=2
Key: Position #2:
paramno=2
name=[0] ""
is_param=1
param_type=2
NULL

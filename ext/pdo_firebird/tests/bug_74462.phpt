--TEST--
PDO_Firebird: Bug #74462 Returns only NULLs for boolean fields
--SKIPIF--
<?php if (!extension_loaded('interbase') || !extension_loaded('pdo_firebird')) die('skip');
?>
--FILE--
<?php
require 'testdb.inc';
$C = new PDO('firebird:dbname='.$test_base, $user, $password) or die;
@$C->exec('drop table atable');
$C->exec('create table atable (id integer not null, abool boolean)');
$C->exec('insert into atable (id, abool) values (1, true)');
$C->exec('insert into atable (id, abool) values (2, false)');
$C->exec('insert into atable (id, abool) values (3, null)');
$S = $C->query('select abool from atable order by id');
$D = $S->fetchAll(PDO::FETCH_COLUMN);
unset($S);
unset($C);
var_dump($D);
?>
--EXPECT--
array(3) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
  [2]=>
  NULL
}

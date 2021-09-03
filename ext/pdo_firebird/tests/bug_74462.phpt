--TEST--
PDO_Firebird: Bug #74462 Returns only NULLs for boolean fields
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php
require 'testdb.inc';

$dbh->exec('recreate table atable (id integer not null, abool boolean)');
$dbh->exec('insert into atable (id, abool) values (1, true)');
$dbh->exec('insert into atable (id, abool) values (2, false)');
$dbh->exec('insert into atable (id, abool) values (3, null)');
$S = $dbh->query('select abool from atable order by id');
$D = $S->fetchAll(PDO::FETCH_COLUMN);
unset($S);
unset($dbh);
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

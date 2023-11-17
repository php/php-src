--TEST--
PDO_Firebird: Bug #74462 Returns only NULLs for boolean fields
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php
require 'testdb.inc';

$dbh->exec('recreate table test74462 (id integer not null, abool boolean)');
$dbh->exec('insert into test74462 (id, abool) values (1, true)');
$dbh->exec('insert into test74462 (id, abool) values (2, false)');
$dbh->exec('insert into test74462 (id, abool) values (3, null)');
$S = $dbh->query('select abool from test74462 order by id');
$D = $S->fetchAll(PDO::FETCH_COLUMN);
unset($S);
unset($dbh);
var_dump($D);
?>
--CLEAN--
<?php
require 'testdb.inc';
@$dbh->exec("DROP TABLE test74462");
unset($dbh);
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

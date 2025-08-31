--TEST--
Bug #80521 (Parameters with underscores no longer recognized)
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--FILE--
<?php
require 'testdb.inc';

$dbh = getDbConnection();
$dbh->exec("CREATE TABLE bug80521 (foo INTEGER)");
var_dump($dbh->prepare("SELECT foo FROM bug80521 WHERE foo = :foo_bar"));
?>
--EXPECTF--
object(PDOStatement)#%d (1) {
  ["queryString"]=>
  string(45) "SELECT foo FROM bug80521 WHERE foo = :foo_bar"
}
--CLEAN--
<?php
require 'testdb.inc';
$dbh = getDbConnection();
@$dbh->exec("DROP TABLE bug80521");
unset($dbh);
?>

--TEST--
PDO_SQLite: Overloading
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once($PDO_TESTS . 'pdo_023_def.inc');

$DB = NULL;

$DB = new PDODatabaseX('sqlite::memory:');
$DB->setAttribute(PDO_ATTR_ERRMODE, PDO_ERRMODE_WARNING);

require_once($PDO_TESTS . 'pdo_023.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
int(1)
int(2)
object(PDODatabaseX)#%d (2) {
  ["test1"]=>
  int(1)
  ["test2"]=>
  int(22)
}
PDODatabaseX::query()
PDOStatementX::__construct()
PDOStatementX::__destruct()
PDODatabaseX::query()
PDOStatementX::__construct()
PDOStatementX::__destruct()
PDODatabaseX::query()
PDOStatementX::__construct()
PDOStatementX::__destruct()
PDODatabaseX::query()
PDOStatementX::__construct()
object(PDOStatementX)#%d (3) {
  ["test1"]=>
  int(1)
  ["queryString"]=>
  string(24) "SELECT val, id FROM test"
  ["test2"]=>
  int(22)
}
array(2) {
  ["A"]=>
  string(1) "0"
  ["B"]=>
  string(1) "1"
}
PDOStatementX::__destruct()
PDODatabaseX::__destruct()
===DONE===

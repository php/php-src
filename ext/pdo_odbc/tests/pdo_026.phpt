--TEST--
PDO_ODBC: Deriving PDO
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once($PDO_TESTS . 'pdo_026.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
string(11) "PDODatabase"
string(12) "PDOStatement"
PDODatabase::query()
PDOStatementX::__construct()
string(13) "PDOStatementX"
string(11) "PDODatabase"
array(3) {
  ["ID"]=>
  string(2) "10"
  ["VAL"]=>
  string(3) "Abc"
  ["VAL2"]=>
  string(3) "zxy"
}
array(3) {
  ["ID"]=>
  string(2) "20"
  ["VAL"]=>
  string(3) "Def"
  ["VAL2"]=>
  string(3) "wvu"
}
array(3) {
  ["ID"]=>
  string(2) "30"
  ["VAL"]=>
  string(3) "Ghi"
  ["VAL2"]=>
  string(3) "tsr"
}
===DONE===
PDODatabase::__destruct()
PDOStatementX::__destruct()

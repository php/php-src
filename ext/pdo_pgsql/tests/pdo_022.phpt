--TEST--
PDO_PGSQL: PDOStatement::getColumnMeta
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once($PDO_TESTS . 'pdo_022.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
array(6) {
  ["pgsql:oid"]=>
  int(23)
  ["native_type"]=>
  string(4) "int4"
  ["name"]=>
  string(2) "id"
  ["len"]=>
  int(4)
  ["precision"]=>
  int(-1)
  ["pdo_type"]=>
  int(1)
}
array(6) {
  ["pgsql:oid"]=>
  int(1043)
  ["native_type"]=>
  string(7) "varchar"
  ["name"]=>
  string(3) "val"
  ["len"]=>
  int(-1)
  ["precision"]=>
  int(14)
  ["pdo_type"]=>
  int(2)
}
array(6) {
  ["pgsql:oid"]=>
  int(1043)
  ["native_type"]=>
  string(7) "varchar"
  ["name"]=>
  string(4) "val2"
  ["len"]=>
  int(-1)
  ["precision"]=>
  int(20)
  ["pdo_type"]=>
  int(2)
}
array(6) {
  ["pgsql:oid"]=>
  int(20)
  ["native_type"]=>
  string(4) "int8"
  ["name"]=>
  string(5) "count"
  ["len"]=>
  int(8)
  ["precision"]=>
  int(-1)
  ["pdo_type"]=>
  int(2)
}
===DONE===

--TEST--
PDO_SQLite2: PDOStatement::getColumnMeta
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
array(7) {
  ["native_type"]=>
  string(7) "integer"
  ["sqlite:decl_type"]=>
  string(3) "INT"
  ["flags"]=>
  array(0) {
  }
  ["name"]=>
  string(2) "id"
  ["len"]=>
  int(-1)
  ["precision"]=>
  int(0)
  ["pdo_type"]=>
  int(2)
}
array(7) {
  ["native_type"]=>
  string(6) "string"
  ["sqlite:decl_type"]=>
  string(11) "VARCHAR(10)"
  ["flags"]=>
  array(0) {
  }
  ["name"]=>
  string(3) "val"
  ["len"]=>
  int(-1)
  ["precision"]=>
  int(0)
  ["pdo_type"]=>
  int(2)
}
array(7) {
  ["native_type"]=>
  string(6) "string"
  ["sqlite:decl_type"]=>
  string(11) "VARCHAR(16)"
  ["flags"]=>
  array(0) {
  }
  ["name"]=>
  string(4) "val2"
  ["len"]=>
  int(-1)
  ["precision"]=>
  int(0)
  ["pdo_type"]=>
  int(2)
}
array(6) {
  ["native_type"]=>
  string(7) "integer"
  ["flags"]=>
  array(0) {
  }
  ["name"]=>
  string(8) "COUNT(*)"
  ["len"]=>
  int(-1)
  ["precision"]=>
  int(0)
  ["pdo_type"]=>
  int(2)
}
===DONE===

--TEST--
PDO_ODBC: PDO_FETCH_UNIQUE
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

$SQL = array('create'=>'CREATE TABLE test(id CHAR(1) NOT NULL PRIMARY KEY, val VARCHAR(10))');

require_once($PDO_TESTS . 'pdo_007.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
array(3) {
  ["A"]=>
  array(1) {
    [0]=>
    string(1) "A"
  }
  ["B"]=>
  array(1) {
    [0]=>
    string(1) "A"
  }
  ["C"]=>
  array(1) {
    [0]=>
    string(1) "C"
  }
}
array(3) {
  ["A"]=>
  array(1) {
    ["VAL"]=>
    string(1) "A"
  }
  ["B"]=>
  array(1) {
    ["VAL"]=>
    string(1) "A"
  }
  ["C"]=>
  array(1) {
    ["VAL"]=>
    string(1) "C"
  }
}
===DONE===

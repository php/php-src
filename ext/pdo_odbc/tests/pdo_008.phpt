--TEST--
PDO_ODBC: PDO_FETCH_UNIQUE conflict
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

$SQL = array('create'=>'CREATE TABLE test(id CHAR(1) NOT NULL PRIMARY KEY, val VARCHAR(10))');

require_once($PDO_TESTS . 'pdo_008.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
array(2) {
  ["A"]=>
  array(1) {
    [0]=>
    string(1) "B"
  }
  ["C"]=>
  array(1) {
    [0]=>
    string(1) "C"
  }
}
===DONE===

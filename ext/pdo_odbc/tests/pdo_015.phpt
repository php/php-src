--TEST--
PDO_ODBC: PDO_FETCH_COLUMN
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc');
?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

$SQL = array(
	'create1'=>'CREATE TABLE test(id int NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(10))'
);

//require_once(dirname(__FILE__).'/../../pdo/tests/pdo.inc');
require_once($PDO_TESTS . 'pdo_015.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
}
array(2) {
  [0]=>
  string(2) "A2"
  [1]=>
  string(2) "B2"
}
array(2) {
  [1]=>
  array(1) {
    [0]=>
    string(1) "A"
  }
  [2]=>
  array(1) {
    [0]=>
    string(1) "A"
  }
}
array(2) {
  [1]=>
  string(1) "A"
  [2]=>
  string(1) "A"
}
array(2) {
  [1]=>
  string(1) "1"
  [2]=>
  string(1) "2"
}
array(2) {
  [1]=>
  string(1) "A"
  [2]=>
  string(1) "A"
}
array(2) {
  [1]=>
  string(2) "A2"
  [2]=>
  string(2) "B2"
}
array(1) {
  ["A"]=>
  array(2) {
    [0]=>
    string(2) "A2"
    [1]=>
    string(2) "B2"
  }
}
===DONE===

--TEST--
PDO_MySQL: PDO_FETCH_COLUMN
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc');
?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once(dirname(__FILE__).'/../../pdo/tests/pdo.inc');
require_once($PDO_TESTS . 'pdo_015.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
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
  int(1)
  [2]=>
  int(2)
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

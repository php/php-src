--TEST--
PDO_Firebird: PDO_FETCH_UNIQUE
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once($PDO_TESTS . 'pdo_007.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
array(3) {
  ["A  "]=>
  array(1) {
    [0]=>
    string(1) "A"
  }
  ["B  "]=>
  array(1) {
    [0]=>
    string(1) "A"
  }
  ["C  "]=>
  array(1) {
    [0]=>
    string(1) "C"
  }
}
array(3) {
  ["A  "]=>
  array(1) {
    ["val"]=>
    string(1) "A"
  }
  ["B  "]=>
  array(1) {
    ["val"]=>
    string(1) "A"
  }
  ["C  "]=>
  array(1) {
    ["val"]=>
    string(1) "C"
  }
}
===DONE===

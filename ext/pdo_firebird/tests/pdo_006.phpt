--TEST--
PDO_Firebird: PDO_FETCH_GROUP
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once($PDO_TESTS . 'pdo_006.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
array(2) {
  ["A"]=>
  array(2) {
    [0]=>
    array(1) {
      [0]=>
      string(1) "1"
    }
    [1]=>
    array(1) {
      [0]=>
      string(1) "2"
    }
  }
  ["C"]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      string(1) "3"
    }
  }
}
array(2) {
  ["A"]=>
  array(2) {
    [0]=>
    array(1) {
      ["id"]=>
      string(1) "1"
    }
    [1]=>
    array(1) {
      ["id"]=>
      string(1) "2"
    }
  }
  ["C"]=>
  array(1) {
    [0]=>
    array(1) {
      ["id"]=>
      string(1) "3"
    }
  }
}
===DONE===

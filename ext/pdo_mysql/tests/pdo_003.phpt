--TEST--
PDO_MySQL: PDO_FETCH_BOTH
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once($PDO_TESTS . 'pdo_003.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
array(3) {
  [0]=>
  array(4) {
    ["id"]=>
    string(1) "1"
    [0]=>
    string(1) "1"
    ["val"]=>
    string(1) "A"
    [1]=>
    string(1) "A"
  }
  [1]=>
  array(4) {
    ["id"]=>
    string(1) "2"
    [0]=>
    string(1) "2"
    ["val"]=>
    string(1) "B"
    [1]=>
    string(1) "B"
  }
  [2]=>
  array(4) {
    ["id"]=>
    string(1) "3"
    [0]=>
    string(1) "3"
    ["val"]=>
    string(1) "C"
    [1]=>
    string(1) "C"
  }
}
===DONE===

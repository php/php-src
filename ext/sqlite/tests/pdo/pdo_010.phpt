--TEST--
PDO_SQLite2: PDO_FETCH_(CLASSTYPE and GROUP/UNIQUE)
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once($PDO_TESTS . 'pdo_010.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test1::__construct()
Test2::__construct()
Test3::__construct()
array(2) {
  ["Group1"]=>
  array(2) {
    [0]=>
    object(stdClass)#%d (2) {
      ["id"]=>
      string(1) "1"
      ["val"]=>
      string(1) "A"
    }
    [1]=>
    object(Test1)#%d (2) {
      ["id"]=>
      string(1) "2"
      ["val"]=>
      string(1) "B"
    }
  }
  ["Group2"]=>
  array(2) {
    [0]=>
    object(Test2)#%d (2) {
      ["id"]=>
      string(1) "3"
      ["val"]=>
      string(1) "C"
    }
    [1]=>
    object(Test3)#%d (2) {
      ["id"]=>
      string(1) "4"
      ["val"]=>
      string(1) "D"
    }
  }
}
Test1::__construct()
Test2::__construct()
Test3::__construct()
array(2) {
  ["Group1"]=>
  object(Test1)#%d (2) {
    ["id"]=>
    string(1) "2"
    ["val"]=>
    string(1) "B"
  }
  ["Group2"]=>
  object(Test3)#%d (2) {
    ["id"]=>
    string(1) "4"
    ["val"]=>
    string(1) "D"
  }
}
===DONE===

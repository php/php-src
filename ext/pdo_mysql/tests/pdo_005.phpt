--TEST--
PDO_MySQL: PDO_FETCH_CLASS
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once($PDO_TESTS . 'pdo_005.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
array(3) {
  [0]=>
  object(stdClass)#%d (3) {
    ["id"]=>
    string(1) "1"
    ["val"]=>
    string(1) "A"
    ["val2"]=>
    string(2) "AA"
  }
  [1]=>
  object(stdClass)#%d (3) {
    ["id"]=>
    string(1) "2"
    ["val"]=>
    string(1) "B"
    ["val2"]=>
    string(2) "BB"
  }
  [2]=>
  object(stdClass)#%d (3) {
    ["id"]=>
    string(1) "3"
    ["val"]=>
    string(1) "C"
    ["val2"]=>
    string(2) "CC"
  }
}
array(3) {
  [0]=>
  object(TestBase)#%d (3) {
    ["id"]=>
    string(1) "1"
    ["val:protected"]=>
    string(1) "A"
    ["val2:private"]=>
    string(2) "AA"
  }
  [1]=>
  object(TestBase)#%d (3) {
    ["id"]=>
    string(1) "2"
    ["val:protected"]=>
    string(1) "B"
    ["val2:private"]=>
    string(2) "BB"
  }
  [2]=>
  object(TestBase)#%d (3) {
    ["id"]=>
    string(1) "3"
    ["val:protected"]=>
    string(1) "C"
    ["val2:private"]=>
    string(2) "CC"
  }
}
TestDerived::__construct(0,1)
TestDerived::__construct(1,2)
TestDerived::__construct(2,3)
array(3) {
  [0]=>
  object(TestDerived)#%d (5) {
    ["row:protected"]=>
    int(0)
    ["id"]=>
    string(1) "1"
    ["val:protected"]=>
    string(1) "A"
    ["val2:private"]=>
    NULL
    ["val2"]=>
    string(2) "AA"
  }
  [1]=>
  object(TestDerived)#%d (5) {
    ["row:protected"]=>
    int(1)
    ["id"]=>
    string(1) "2"
    ["val:protected"]=>
    string(1) "B"
    ["val2:private"]=>
    NULL
    ["val2"]=>
    string(2) "BB"
  }
  [2]=>
  object(TestDerived)#%d (5) {
    ["row:protected"]=>
    int(2)
    ["id"]=>
    string(1) "3"
    ["val:protected"]=>
    string(1) "C"
    ["val2:private"]=>
    NULL
    ["val2"]=>
    string(2) "CC"
  }
}
===DONE===

--TEST--
PDO_ODBC: PDO_FETCH_CLASS
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

$SQL = array(
	'create'=>'CREATE TABLE test(id int NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(10))'
);

require_once($PDO_TESTS . 'pdo_005.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
array(3) {
  [0]=>
  object(stdClass)#%d (3) {
    ["ID"]=>
    string(1) "1"
    ["VAL"]=>
    string(1) "A"
    ["VAL2"]=>
    string(2) "AA"
  }
  [1]=>
  object(stdClass)#%d (3) {
    ["ID"]=>
    string(1) "2"
    ["VAL"]=>
    string(1) "B"
    ["VAL2"]=>
    string(2) "BB"
  }
  [2]=>
  object(stdClass)#%d (3) {
    ["ID"]=>
    string(1) "3"
    ["VAL"]=>
    string(1) "C"
    ["VAL2"]=>
    string(2) "CC"
  }
}
array(3) {
  [0]=>
  object(TestBase)#%d (6) {
    ["id"]=>
    NULL
    ["val:protected"]=>
    NULL
    ["val2:private"]=>
    NULL
    ["ID"]=>
    string(1) "1"
    ["VAL"]=>
    string(1) "A"
    ["VAL2"]=>
    string(2) "AA"
  }
  [1]=>
  object(TestBase)#%d (6) {
    ["id"]=>
    NULL
    ["val:protected"]=>
    NULL
    ["val2:private"]=>
    NULL
    ["ID"]=>
    string(1) "2"
    ["VAL"]=>
    string(1) "B"
    ["VAL2"]=>
    string(2) "BB"
  }
  [2]=>
  object(TestBase)#%d (6) {
    ["id"]=>
    NULL
    ["val:protected"]=>
    NULL
    ["val2:private"]=>
    NULL
    ["ID"]=>
    string(1) "3"
    ["VAL"]=>
    string(1) "C"
    ["VAL2"]=>
    string(2) "CC"
  }
}
TestDerived::__construct(0,)
TestDerived::__construct(1,)
TestDerived::__construct(2,)
array(3) {
  [0]=>
  object(TestDerived)#%d (7) {
    ["row:protected"]=>
    int(0)
    ["id"]=>
    NULL
    ["val:protected"]=>
    NULL
    ["val2:private"]=>
    NULL
    ["ID"]=>
    string(1) "1"
    ["VAL"]=>
    string(1) "A"
    ["VAL2"]=>
    string(2) "AA"
  }
  [1]=>
  object(TestDerived)#%d (7) {
    ["row:protected"]=>
    int(1)
    ["id"]=>
    NULL
    ["val:protected"]=>
    NULL
    ["val2:private"]=>
    NULL
    ["ID"]=>
    string(1) "2"
    ["VAL"]=>
    string(1) "B"
    ["VAL2"]=>
    string(2) "BB"
  }
  [2]=>
  object(TestDerived)#%d (7) {
    ["row:protected"]=>
    int(2)
    ["id"]=>
    NULL
    ["val:protected"]=>
    NULL
    ["val2:private"]=>
    NULL
    ["ID"]=>
    string(1) "3"
    ["VAL"]=>
    string(1) "C"
    ["VAL2"]=>
    string(2) "CC"
  }
}
===DONE===

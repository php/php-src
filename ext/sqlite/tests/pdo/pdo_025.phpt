--TEST--
PDO_SQLite2: PDO_FETCH_INTO
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once($PDO_TESTS . 'pdo_025.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
object(Test)#%d (3) {
  ["id"]=>
  string(2) "10"
  ["val"]=>
  string(3) "Abc"
  ["val2"]=>
  string(3) "zxy"
}
object(Test)#%d (3) {
  ["id"]=>
  string(2) "20"
  ["val"]=>
  string(3) "Def"
  ["val2"]=>
  string(3) "wvu"
}
object(Test)#%d (3) {
  ["id"]=>
  string(2) "30"
  ["val"]=>
  string(3) "Ghi"
  ["val2"]=>
  string(3) "tsr"
}
object(Test)#%d (3) {
  ["id"]=>
  string(2) "40"
  ["val"]=>
  string(3) "Jkl"
  ["val2"]=>
  string(3) "qpo"
}
object(Test)#%d (3) {
  ["id"]=>
  string(2) "50"
  ["val"]=>
  string(3) "Mno"
  ["val2"]=>
  string(3) "nml"
}
object(Test)#%d (3) {
  ["id"]=>
  string(2) "60"
  ["val"]=>
  string(3) "Pqr"
  ["val2"]=>
  string(3) "kji"
}
===FAIL===

Fatal error: Cannot access protected property Fail::$id in %spdo_025.inc on line %d

--TEST--
PDO_ODBC: PDOStatement is Traversable
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

$SQL = array('create'=>'CREATE TABLE test(id INT NOT NULL PRIMARY KEY, val VARCHAR(10), grp VARCHAR(10))');

require_once($PDO_TESTS . 'pdo_013.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
array(2) {
  [0]=>
  string(1) "A"
  [1]=>
  string(6) "Group1"
}
array(2) {
  [0]=>
  string(1) "B"
  [1]=>
  string(6) "Group2"
}
Test::__construct(N/A)
object(Test)#%d (2) {
  ["VAL"]=>
  string(1) "A"
  ["GRP"]=>
  string(6) "Group1"
}
Test::__construct(N/A)
object(Test)#%d (2) {
  ["VAL"]=>
  string(1) "B"
  ["GRP"]=>
  string(6) "Group2"
}
Test::__construct(WOW)
object(Test)#%d (2) {
  ["VAL"]=>
  string(1) "A"
  ["GRP"]=>
  string(6) "Group1"
}
Test::__construct(WOW)
object(Test)#%d (2) {
  ["VAL"]=>
  string(1) "B"
  ["GRP"]=>
  string(6) "Group2"
}
===DONE===

--TEST--
PDO_Firebird: PDOStatement and SPL Iterators
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc');
if (!extension_loaded('SPL')) die('skip SPL not available');
?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once($PDO_TESTS . 'pdo_014.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test::__construct(WOW)
object(Test)#4 (2) {
  ["val"]=>
  string(1) "A"
  ["grp"]=>
  string(6) "Group1"
}
Test::__construct(WOW)
object(Test)#6 (2) {
  ["val"]=>
  string(1) "B"
  ["grp"]=>
  string(6) "Group2"
}
NULL
bool(false)
PDOStatementAggregate::__construct
PDOStatementAggregate::getIterator
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
===DONE===

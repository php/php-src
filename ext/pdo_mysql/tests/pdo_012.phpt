--TEST--
PDO_MySQ: PDOStatement::setFetchMode
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once($PDO_TESTS . 'pdo_012.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "A"
    [1]=>
    string(6) "Group1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "B"
    [1]=>
    string(6) "Group2"
  }
}
Test::__construct(N/A)
Test::__construct(N/A)
array(2) {
  [0]=>
  object(Test)#%d (2) {
    ["val"]=>
    string(1) "A"
    ["grp"]=>
    string(6) "Group1"
  }
  [1]=>
  object(Test)#%d (2) {
    ["val"]=>
    string(1) "B"
    ["grp"]=>
    string(6) "Group2"
  }
}
Test::__construct(Changed)
Test::__construct(Changed)
array(2) {
  [0]=>
  object(Test)#%d (2) {
    ["val"]=>
    string(1) "A"
    ["grp"]=>
    string(6) "Group1"
  }
  [1]=>
  object(Test)#%d (2) {
    ["val"]=>
    string(1) "B"
    ["grp"]=>
    string(6) "Group2"
  }
}
===DONE===

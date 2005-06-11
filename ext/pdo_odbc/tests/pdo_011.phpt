--TEST--
PDO_ODBC: PDO_FETCH_FUNC and statement overloading
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

$SQL = array('create'=>'CREATE TABLE test(id INT NOT NULL PRIMARY KEY, val VARCHAR(10), grp VARCHAR(10))');

require_once($PDO_TESTS . 'pdo_011.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test1::__construct(0,0)
test(1,N/A)
test(2,N/A)
test(3,N/A)
test(4,N/A)
array(2) {
  ["Group1"]=>
  array(2) {
    [0]=>
    array(1) {
      [1]=>
      string(3) "N/A"
    }
    [1]=>
    array(1) {
      [2]=>
      string(3) "N/A"
    }
  }
  ["Group2"]=>
  array(2) {
    [0]=>
    array(1) {
      [3]=>
      string(3) "N/A"
    }
    [1]=>
    array(1) {
      [4]=>
      string(3) "N/A"
    }
  }
}
test(1,A)
test(2,B)
test(3,C)
test(4,D)
array(4) {
  [0]=>
  array(1) {
    [1]=>
    string(1) "A"
  }
  [1]=>
  array(1) {
    [2]=>
    string(1) "B"
  }
  [2]=>
  array(1) {
    [3]=>
    string(1) "C"
  }
  [3]=>
  array(1) {
    [4]=>
    string(1) "D"
  }
}
Test1::factory(1,A)
Test1::__construct(1,A)
Test1::factory(2,B)
Test1::__construct(2,B)
Test1::factory(3,C)
Test1::__construct(3,C)
Test1::factory(4,D)
Test1::__construct(4,D)
array(4) {
  [0]=>
  object(Test1)#%d (2) {
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
  [2]=>
  object(Test1)#%d (2) {
    ["id"]=>
    string(1) "3"
    ["val"]=>
    string(1) "C"
  }
  [3]=>
  object(Test1)#%d (2) {
    ["id"]=>
    string(1) "4"
    ["val"]=>
    string(1) "D"
  }
}
Test1::factory(1,A)
Test1::__construct(1,A)
Test1::factory(2,B)
Test1::__construct(2,B)
Test1::factory(3,C)
Test1::__construct(3,C)
Test1::factory(4,D)
Test1::__construct(4,D)
array(4) {
  [0]=>
  object(Test1)#%d (2) {
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
  [2]=>
  object(Test1)#%d (2) {
    ["id"]=>
    string(1) "3"
    ["val"]=>
    string(1) "C"
  }
  [3]=>
  object(Test1)#%d (2) {
    ["id"]=>
    string(1) "4"
    ["val"]=>
    string(1) "D"
  }
}
DerivedStatement::__construct(Overloaded)
string(16) "DerivedStatement"
DerivedStatement::retrieve(1,A)
DerivedStatement::retrieve(2,B)
DerivedStatement::retrieve(3,C)
DerivedStatement::retrieve(4,D)
array(4) {
  [0]=>
  array(1) {
    [1]=>
    string(1) "A"
  }
  [1]=>
  array(1) {
    [2]=>
    string(1) "B"
  }
  [2]=>
  array(1) {
    [3]=>
    string(1) "C"
  }
  [3]=>
  array(1) {
    [4]=>
    string(1) "D"
  }
}
===DONE===

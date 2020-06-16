--TEST--
Testing array dereference and references
--FILE--
<?php

error_reporting(E_ALL);

function &foo(&$foo) {
    return $foo;
}

$a = array(1);
foo($a)[0] = 2;
var_dump($a);

foo($a)[] = 3;
var_dump($a);

?>
--EXPECT--
array(1) {
  [0]=>
  int(2)
}
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}

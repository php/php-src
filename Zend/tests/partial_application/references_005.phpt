--TEST--
PFA inherits return by ref
--FILE--
<?php

function &foo(&$a) {
    return $a;
}

$f = foo(?);
$a = [];
$b = &$f($a);
$a[0] = 1;

var_dump($a, $b);

?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(1)
}

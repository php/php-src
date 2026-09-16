--TEST--
list() with array reference
--FILE--
<?php

$arr = array(2, 1);
$b =& $arr;

list(,$a) = $b;

var_dump($a, $b);

?>
--EXPECT--
int(1)
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(1)
}

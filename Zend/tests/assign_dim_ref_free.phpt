--TEST--
Assigning rc=1 reference to next dim
--FILE--
<?php
var_dump($ary[] = [&$x] = $x);
var_dump($x);
?>
--EXPECT--
array(1) {
  [0]=>
  &NULL
}
NULL

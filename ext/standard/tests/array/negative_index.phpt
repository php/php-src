--TEST--
Test arrays starting with negative indices
--FILE--
<?php

$a = array_fill(-2, 3, true);
$b = [-2 => true, true, true];
$c = ["string" => true, -2 => true, true, true];
unset($c["string"]);
$d[-2] = true;
$d[] = true;
$d[] = true;
$e = [-2 => false];
array_pop($e);
$e[] = true;
$e[] = true;
$e[] = true;

var_dump($a === $b && $b === $c && $c === $d && $d == $e);
var_dump($a);
?>
--EXPECT--
bool(true)
array(3) {
  [-2]=>
  bool(true)
  [-1]=>
  bool(true)
  [0]=>
  bool(true)
}

--TEST--
Bug #81216_2: Nullsafe operator leaks dynamic property name
--FILE--
<?php
$a = [null];
$a[1] = $a[0]?->x;
var_dump($a);
?>
--EXPECT--
array(2) {
  [0]=>
  NULL
  [1]=>
  NULL
}

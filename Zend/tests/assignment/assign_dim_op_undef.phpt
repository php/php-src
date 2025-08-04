--TEST--
Compound array assign with undefined variables
--FILE--
<?php
$a[$b] += 1;
var_dump($a);
?>
--EXPECTF--
Warning: Undefined variable $a in %s on line %d

Warning: Undefined variable $b in %s on line %d

Warning: Undefined array key "" in %s on line %d
array(1) {
  [""]=>
  int(1)
}

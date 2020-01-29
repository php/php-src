--TEST--
Generator wit type check
--FILE--
<?php
function gen(array $a) { yield; }
gen(42);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: gen() expects argument #1 ($a) to be of type array, int given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %sgenerator_with_type_check.php(3): gen(42)
#1 {main}
  thrown in %sgenerator_with_type_check.php on line 2

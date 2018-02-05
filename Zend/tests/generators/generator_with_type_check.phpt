--TEST--
Generator wit type check
--FILE--
<?php
function gen(array $a) { yield; }
gen(42);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Argument 1 passed to gen() must be of the type array, int given, called in %sgenerator_with_type_check.php on line 3 and defined in %sgenerator_with_type_check.php:2
Stack trace:
#0 %sgenerator_with_type_check.php(3): gen(42)
#1 {main}
  thrown in %sgenerator_with_type_check.php on line 2

--TEST--
Unpacking arrays in constant expression
--FILE--
<?php

const A = [...[1, 2, 3]];
const B = [...['a'=>1, 'b'=>2, 'c'=>3]];
const C = [...new ArrayObject()];

?>
--EXPECTF--
Fatal error: Uncaught Error: Only arrays can be unpacked in constant expression in %sgh9769.php:5
Stack trace:
#0 {main}
  thrown in %sgh9769.php on line 5

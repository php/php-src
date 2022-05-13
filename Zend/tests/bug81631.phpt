--TEST--
Bug #81631: ::class with dynamic class name may yield wrong line number
--FILE--
<?php
$a = 0;
var_dump($b::class);
?>
--EXPECTF--
Warning: Undefined variable $b in %s on line 3

Fatal error: Uncaught TypeError: Cannot use "::class" on value of type null in %s:3
Stack trace:
#0 {main}
  thrown in %s on line 3

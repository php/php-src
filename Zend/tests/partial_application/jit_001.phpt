--TEST--
PFA JIT 001
--FILE--
<?php
(function ($a,$b) { var_dump($a, $b); })(1, ...)(2);
?>
--EXPECT--
int(1)
int(2)

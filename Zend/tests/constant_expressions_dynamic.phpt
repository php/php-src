--TEST--
Dynamic Constant Expressions
--FILE--
<?php
const FOO = 1;
const BAR = FOO | 2;

echo BAR;
?>
--EXPECTF--
3

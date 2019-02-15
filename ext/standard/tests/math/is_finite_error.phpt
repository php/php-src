--TEST--
Test is_finite() - wrong params test is_finite()
--FILE--
<?php
is_finite();
is_finite(23,2,true);
?>
--EXPECTF--
Warning: is_finite() expects exactly 1 parameter, 0 given in %s on line 2

Warning: is_finite() expects exactly 1 parameter, 3 given in %s on line 3

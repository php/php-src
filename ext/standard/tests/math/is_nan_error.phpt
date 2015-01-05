--TEST--
Test is_nan() - wrong params test is_nan()
--FILE--
<?php
is_nan();
is_nan(23,2,true);
?>
--EXPECTF--

Warning: is_nan() expects exactly 1 parameter, 0 given in %s on line 2

Warning: is_nan() expects exactly 1 parameter, 3 given in %s on line 3





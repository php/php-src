--TEST--
round() with different rounding modes
--FILE--
<?php
var_dump (round (2.5, 0, PHP_ROUND_HALF_UP));
var_dump (round (2.5, 0, PHP_ROUND_HALF_DOWN));
var_dump (round (2.5, 0, PHP_ROUND_HALF_EVEN));
var_dump (round (2.5, 0, PHP_ROUND_HALF_ODD));
var_dump (round (-2.5, 0, PHP_ROUND_HALF_UP));
var_dump (round (-2.5, 0, PHP_ROUND_HALF_DOWN));
var_dump (round (-2.5, 0, PHP_ROUND_HALF_EVEN));
var_dump (round (-2.5, 0, PHP_ROUND_HALF_ODD));
var_dump (round (3.5, 0, PHP_ROUND_HALF_UP));
var_dump (round (3.5, 0, PHP_ROUND_HALF_DOWN));
var_dump (round (3.5, 0, PHP_ROUND_HALF_EVEN));
var_dump (round (3.5, 0, PHP_ROUND_HALF_ODD));
var_dump (round (-3.5, 0, PHP_ROUND_HALF_UP));
var_dump (round (-3.5, 0, PHP_ROUND_HALF_DOWN));
var_dump (round (-3.5, 0, PHP_ROUND_HALF_EVEN));
var_dump (round (-3.5, 0, PHP_ROUND_HALF_ODD));
?>
--EXPECT--
float(3)
float(2)
float(2)
float(3)
float(-3)
float(-2)
float(-2)
float(-3)
float(4)
float(3)
float(4)
float(3)
float(-4)
float(-3)
float(-4)
float(-3)

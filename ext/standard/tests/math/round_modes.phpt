--TEST--
round() with different rounding modes
--FILE--
<?php
var_dump (round (2.5, 0, PHP_ROUND_HALF_UP));
var_dump (round (2.5, 0, PHP_ROUND_HALF_DOWN));
var_dump (round (2.5, 0, PHP_ROUND_HALF_EVEN));
var_dump (round (2.5, 0, PHP_ROUND_HALF_ODD));
var_dump (round (2.5, 0, PHP_ROUND_CEILING));
var_dump (round (2.5, 0, PHP_ROUND_FLOOR));
var_dump (round (2.5, 0, PHP_ROUND_TOWARD_ZERO));
var_dump (round (2.5, 0, PHP_ROUND_AWAY_FROM_ZERO));
var_dump (round (-2.5, 0, PHP_ROUND_HALF_UP));
var_dump (round (-2.5, 0, PHP_ROUND_HALF_DOWN));
var_dump (round (-2.5, 0, PHP_ROUND_HALF_EVEN));
var_dump (round (-2.5, 0, PHP_ROUND_HALF_ODD));
var_dump (round (-2.5, 0, PHP_ROUND_CEILING));
var_dump (round (-2.5, 0, PHP_ROUND_FLOOR));
var_dump (round (-2.5, 0, PHP_ROUND_TOWARD_ZERO));
var_dump (round (-2.5, 0, PHP_ROUND_AWAY_FROM_ZERO));
var_dump (round (3.5, 0, PHP_ROUND_HALF_UP));
var_dump (round (3.5, 0, PHP_ROUND_HALF_DOWN));
var_dump (round (3.5, 0, PHP_ROUND_HALF_EVEN));
var_dump (round (3.5, 0, PHP_ROUND_HALF_ODD));
var_dump (round (3.5, 0, PHP_ROUND_CEILING));
var_dump (round (3.5, 0, PHP_ROUND_FLOOR));
var_dump (round (3.5, 0, PHP_ROUND_TOWARD_ZERO));
var_dump (round (3.5, 0, PHP_ROUND_AWAY_FROM_ZERO));
var_dump (round (-3.5, 0, PHP_ROUND_HALF_UP));
var_dump (round (-3.5, 0, PHP_ROUND_HALF_DOWN));
var_dump (round (-3.5, 0, PHP_ROUND_HALF_EVEN));
var_dump (round (-3.5, 0, PHP_ROUND_HALF_ODD));
var_dump (round (-3.5, 0, PHP_ROUND_CEILING));
var_dump (round (-3.5, 0, PHP_ROUND_FLOOR));
var_dump (round (-3.5, 0, PHP_ROUND_TOWARD_ZERO));
var_dump (round (-3.5, 0, PHP_ROUND_AWAY_FROM_ZERO));
?>
--EXPECT--
float(3)
float(2)
float(2)
float(3)
float(3)
float(2)
float(2)
float(3)
float(-3)
float(-2)
float(-2)
float(-3)
float(-2)
float(-3)
float(-2)
float(-3)
float(4)
float(3)
float(4)
float(3)
float(4)
float(3)
float(3)
float(4)
float(-4)
float(-3)
float(-4)
float(-3)
float(-3)
float(-4)
float(-3)
float(-4)

--TEST--
Bug #12143 round() with large FP
--FILE--
<?php
var_dump(round(0.49999999999999994, 0, PHP_ROUND_HALF_UP));
var_dump(round(0.49999999999999994, 0, PHP_ROUND_HALF_DOWN));
var_dump(round(0.49999999999999994, 0, PHP_ROUND_HALF_EVEN));
var_dump(round(0.49999999999999994, 0, PHP_ROUND_HALF_ODD));
var_dump(round(-0.49999999999999994, 0, PHP_ROUND_HALF_UP));
var_dump(round(-0.49999999999999994, 0, PHP_ROUND_HALF_DOWN));
var_dump(round(-0.49999999999999994, 0, PHP_ROUND_HALF_EVEN));
var_dump(round(-0.49999999999999994, 0, PHP_ROUND_HALF_ODD));
var_dump(round(0.5000000000000004, 0, PHP_ROUND_HALF_UP));
var_dump(round(0.5000000000000004, 0, PHP_ROUND_HALF_DOWN));
var_dump(round(0.5000000000000004, 0, PHP_ROUND_HALF_EVEN));
var_dump(round(0.5000000000000004, 0, PHP_ROUND_HALF_ODD));
var_dump(round(-0.5000000000000004, 0, PHP_ROUND_HALF_UP));
var_dump(round(-0.5000000000000004, 0, PHP_ROUND_HALF_DOWN));
var_dump(round(-0.5000000000000004, 0, PHP_ROUND_HALF_EVEN));
var_dump(round(-0.5000000000000004, 0, PHP_ROUND_HALF_ODD));
?>
--EXPECT--
float(0)
float(0)
float(0)
float(0)
float(0)
float(0)
float(0)
float(0)
float(1)
float(1)
float(1)
float(1)
float(-1)
float(-1)
float(-1)
float(-1)

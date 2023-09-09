--TEST--
Bug #12143 round() with large FP
--FILE--
<?php
var_dump(round(0.49999999999999994, 0, PHP_ROUND_HALF_UP) == 0);
var_dump(round(0.49999999999999994, 0, PHP_ROUND_HALF_DOWN) == 0);
var_dump(round(0.49999999999999994, 0, PHP_ROUND_HALF_EVEN) == 0);
var_dump(round(0.49999999999999994, 0, PHP_ROUND_HALF_ODD) == 0);
var_dump(round(-0.49999999999999994, 0, PHP_ROUND_HALF_UP) == -0);
var_dump(round(-0.49999999999999994, 0, PHP_ROUND_HALF_DOWN) == 0);
var_dump(round(-0.49999999999999994, 0, PHP_ROUND_HALF_EVEN) == 0);
var_dump(round(-0.49999999999999994, 0, PHP_ROUND_HALF_ODD) == 0);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

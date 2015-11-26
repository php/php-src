--TEST--
round() works correctly for large exponents
--FILE--
<?php
var_dump (2e-22 == round (2e-22, 22, PHP_ROUND_HALF_UP));
var_dump (1e-22 == round (1e-22, 22, PHP_ROUND_HALF_UP));
var_dump (2e-23 == round (2e-23, 23, PHP_ROUND_HALF_UP));
var_dump (1e-23 == round (1e-23, 23, PHP_ROUND_HALF_UP));
var_dump (2e-24 == round (2e-24, 24, PHP_ROUND_HALF_UP));
var_dump (1e-24 == round (1e-24, 24, PHP_ROUND_HALF_UP));
var_dump (2e22 == round (2e22, -22, PHP_ROUND_HALF_UP));
var_dump (1e22 == round (1e22, -22, PHP_ROUND_HALF_UP));
var_dump (2e23 == round (2e23, -23, PHP_ROUND_HALF_UP));
var_dump (1e23 == round (1e23, -23, PHP_ROUND_HALF_UP));
var_dump (2e24 == round (2e24, -24, PHP_ROUND_HALF_UP));
var_dump (1e24 == round (1e24, -24, PHP_ROUND_HALF_UP));
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
bool(true)
bool(true)
bool(true)
bool(true)

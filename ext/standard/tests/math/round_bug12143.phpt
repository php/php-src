--TEST--
Bug #12143 round with large precision
--FILE--
<?php
var_dump(round(0.49999999999999994, 0, PHP_ROUND_HALF_UP) == 0);
var_dump(round(1.700000000000145, 13, PHP_ROUND_HALF_UP) == 1.7000000000001);
?>
--EXPECT--
bool(true)
bool(true)

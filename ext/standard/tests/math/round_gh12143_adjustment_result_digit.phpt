--TEST--
Fix GH-12143: Improved handling of adjusting result digits
--FILE--
<?php
var_dump(round(1.700000000000145, 13, PHP_ROUND_HALF_UP));
var_dump(round(-1.700000000000145, 13, PHP_ROUND_HALF_UP));
?>
--EXPECT--
float(1.7000000000001)
float(-1.7000000000001)

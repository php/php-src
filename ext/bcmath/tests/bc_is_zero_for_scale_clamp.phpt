--TEST--
bc_is_zero_for_scale clamps scale to n_scale (Number::compare opposite signs)
--EXTENSIONS--
bcmath
--FILE--
<?php
$shortZero = (new BcMath\Number('1.0'))->sub('1.0');
$longNegative = new BcMath\Number('-0.' . str_repeat('0', 64) . '1');
var_dump($shortZero->compare($longNegative, 64));
?>
--EXPECT--
int(0)

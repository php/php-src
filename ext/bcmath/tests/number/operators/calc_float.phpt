--TEST--
BcMath\Number calc float by operator
--EXTENSIONS--
bcmath
--FILE--
<?php
$num = new BcMath\Number(100);

$num + 1.01;
$num - 1.01;
$num * 1.01;
$num / 1.01;
$num % 1.01;
$num ** 1.01;
?>
--EXPECTF--
Deprecated: Implicit conversion from float 1.01 to int loses precision in %s

Deprecated: Implicit conversion from float 1.01 to int loses precision in %s

Deprecated: Implicit conversion from float 1.01 to int loses precision in %s

Deprecated: Implicit conversion from float 1.01 to int loses precision in %s

Deprecated: Implicit conversion from float 1.01 to int loses precision in %s

Deprecated: Implicit conversion from float 1.01 to int loses precision in %s

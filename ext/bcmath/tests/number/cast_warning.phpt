--TEST--
BcMath\Number cast warning
--EXTENSIONS--
bcmath
--FILE--
<?php
$num = new BcMath\Number(1);
(int) $num;
(float) $num;
?>
--EXPECTF--
Warning: Object of class BcMath\Number could not be converted to int in %s on line %d

Warning: Object of class BcMath\Number could not be converted to float in %s on line %d

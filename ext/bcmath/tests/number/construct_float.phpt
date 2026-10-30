--TEST--
BcMath\Number construct float
--EXTENSIONS--
bcmath
--FILE--
<?php
$num = new BcMath\Number(0.1234);
var_dump($num);
?>
--EXPECTF--
Deprecated: Implicit conversion from float 0.1234 to int loses precision in %s
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

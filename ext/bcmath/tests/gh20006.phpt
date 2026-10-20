--TEST--
GH-20006 (Power of 0 of BcMath number causes crash)
--EXTENSIONS--
bcmath
--FILE--
<?php
$n = new BcMath\Number("0");
var_dump(pow($n, 2));
?>
--EXPECTF--
object(BcMath\Number)#%d (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

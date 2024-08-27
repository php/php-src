--TEST--
BcMath\Number cast warning
--EXTENSIONS--
bcmath
--FILE--
<?php
$num = new BcMath\Number(1);
try {
    (int) $num;
    (float) $num;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
Warning: Object of class BcMath\Number could not be converted to int in %s

Warning: Object of class BcMath\Number could not be converted to float in %s

--TEST--
GH-18641 (Accessing a BcMath\Number property by ref crashes)
--EXTENSIONS--
bcmath
--FILE--
<?php
$a = new BCMath\Number("1");
$fusion = $a;
$x = &$fusion->value;
var_dump($x);
?>
--EXPECT--
string(1) "1"

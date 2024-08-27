--TEST--
BcMath\Number properties isset
--EXTENSIONS--
bcmath
--FILE--
<?php

$num = new BcMath\Number(1);

var_dump(isset($num->value));
var_dump(isset($num->scale));
?>
--EXPECT--
bool(true)
bool(true)

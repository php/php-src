--TEST--
Unary minus constant expression consistency
--FILE--
<?php

const ZERO = 0.0;
const MINUS_ZERO = -ZERO;
$minus_zero = -ZERO;

var_dump(MINUS_ZERO);
var_dump($minus_zero);

?>
--EXPECT--
float(-0)
float(-0)

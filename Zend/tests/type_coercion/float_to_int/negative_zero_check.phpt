--TEST--
Negative 0 check
--FILE--
<?php

$negativeZero = -0.0;
var_dump($negativeZero);
var_dump($negativeZero === (float)(int)$negativeZero);
var_dump($negativeZero === 0.0);

?>
--EXPECT--
float(-0)
bool(true)
bool(true)

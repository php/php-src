--TEST--
Parentheses-less form of short closure
--FILE--
<?php

$y = 2;
$fn = $x ==> $x * $y;
var_dump($fn(24));

?>
--EXPECT--
int(48)

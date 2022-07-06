--TEST--
Bug #76717: var_export() does not create a parsable value for PHP_INT_MIN
--FILE--
<?php

$min = eval('return '.var_export(PHP_INT_MIN, true).';');
$max = eval('return '.var_export(PHP_INT_MAX, true).';');
var_dump($min === PHP_INT_MIN);
var_dump($max === PHP_INT_MAX);

?>
--EXPECT--
bool(true)
bool(true)

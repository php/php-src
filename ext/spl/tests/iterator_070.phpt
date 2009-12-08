--TEST--
SPL: RecursiveIteratorIterator - Ensure that non-overriden methods execute problem free.
--FILE--
<?php

$array = array();
$recArrIt = new RecursiveArrayIterator($array);

$recItIt = new RecursiveIteratorIterator($recArrIt);

var_dump($recItIt->beginIteration());
var_dump($recItIt->endIteration());
var_dump($recItIt->nextElement());

?>

--EXPECTF--
NULL
NULL
NULL
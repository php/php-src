--TEST--
Test nullsafe operator in nested delayed dims 2
--FILE--
<?php

$foo = (object) ['bar' => 0];
$array = [[null]];
var_dump($array[0][$foo->bar]?->baz);

?>
--EXPECT--
NULL

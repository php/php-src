--TEST--
Bug #53033: Mathematical operations convert objects to integers
--EXTENSIONS--
simplexml
--FILE--
<?php

$x = simplexml_load_string('<x>2.5</x>');
var_dump($x*1);
// type of other operand is irrelevant
var_dump($x*1.0);

// strings behave differently
$y = '2.5';
var_dump($y*1);
var_dump((string)$x*1);

?>
--EXPECT--
float(2.5)
float(2.5)
float(2.5)
float(2.5)

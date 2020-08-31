--TEST--
Assign concat of array and empty string
--FILE--
<?php

$a = [0];
$a .= '';
var_dump($a);

?>
--EXPECTF--
Notice: Array to string conversion in %s on line %d
string(5) "Array"

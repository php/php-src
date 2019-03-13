--TEST--
Trying to use lambda in array offset
--FILE--
<?php

$test[function(){}] = 1;
$a{function() { }} = 1;

?>
--EXPECTF--

Deprecated: Array and string offset access syntax with curly braces is deprecated in %s line %d

Warning: Illegal offset type in %s on line %d

Warning: Illegal offset type in %s on line %d

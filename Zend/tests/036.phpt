--TEST--
Trying to use lambda in array offset
--FILE--
<?php

$test[function(){}] = 1;
$a{function() { }} = 1;

?>
--EXPECTF--
Warning: Illegal offset type in %s on line %d

Warning: Illegal offset type in %s on line %d

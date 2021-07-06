--TEST--
float type cannot take part in an intersection type
--FILE--
<?php

function foo(): float&Iterator {}

?>
--EXPECTF--
Fatal error: Type float cannot be part of an intersection type in %s on line %d

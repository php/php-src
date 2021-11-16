--TEST--
int type cannot take part in an intersection type
--FILE--
<?php

function foo(): int&Iterator {}

?>
--EXPECTF--
Fatal error: Type int cannot be part of an intersection type in %s on line %d

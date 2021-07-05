--TEST--
array type cannot take part in an intersection type
--FILE--
<?php

function foo(): array&Iterator {}

?>
--EXPECTF--
Fatal error: Type array cannot be part of an intersection type in %s on line %d

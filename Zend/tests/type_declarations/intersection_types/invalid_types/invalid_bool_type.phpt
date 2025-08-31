--TEST--
bool type cannot take part in an intersection type
--FILE--
<?php

function foo(): bool&Iterator {}

?>
--EXPECTF--
Fatal error: Type bool cannot be part of an intersection type in %s on line %d

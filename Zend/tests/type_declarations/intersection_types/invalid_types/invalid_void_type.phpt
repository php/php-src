--TEST--
void type cannot take part in an intersection type
--FILE--
<?php

function foo(): void&Iterator {}

?>
--EXPECTF--
Fatal error: Type void cannot be part of an intersection type in %s on line %d

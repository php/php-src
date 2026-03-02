--TEST--
object type cannot take part in an intersection type
--FILE--
<?php

function foo(): object&Iterator {}

?>
--EXPECTF--
Fatal error: Type object cannot be part of an intersection type in %s on line %d

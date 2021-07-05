--TEST--
null type cannot take part in an intersection type
--FILE--
<?php

function foo(): null&Iterator {}

?>
--EXPECTF--
Fatal error: Type null cannot be part of an intersection type in %s on line %d

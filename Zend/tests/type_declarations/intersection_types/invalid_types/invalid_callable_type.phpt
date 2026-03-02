--TEST--
callable type cannot take part in an intersection type
--FILE--
<?php

function foo(): callable&Iterator {}

?>
--EXPECTF--
Fatal error: Type callable cannot be part of an intersection type in %s on line %d

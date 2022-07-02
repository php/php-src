--TEST--
never type cannot take part in an intersection type
--FILE--
<?php

function foo(): never&Iterator {}

?>
--EXPECTF--
Fatal error: Type never cannot be part of an intersection type in %s on line %d

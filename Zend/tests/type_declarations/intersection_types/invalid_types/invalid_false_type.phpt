--TEST--
false type cannot take part in an intersection type
--FILE--
<?php

function foo(): false&Iterator {}

?>
--EXPECTF--
Fatal error: Type false cannot be part of an intersection type in %s on line %d

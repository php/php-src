--TEST--
string type cannot take part in an intersection type
--FILE--
<?php

function foo(): string&Iterator {}

?>
--EXPECTF--
Fatal error: Type string cannot be part of an intersection type in %s on line %d

--TEST--
Test nullable mixed parameter type.
--FILE--
<?php

function test(?mixed $arg) {
}

--EXPECTF--
Fatal error: Mixed type cannot be nullable in %smixed_nullable_parameter.php on line %d

--TEST--
Test nullable mixed return type.
--FILE--
<?php

function test($arg): ?mixed {
}

--EXPECTF--
Fatal error: Mixed type cannot be nullable in %smixed_nullable_return.php on line %d

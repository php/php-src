--TEST--
Nullsafe operator does not support BP_VAR_W
--FILE--
<?php

function &test($foo) {
    return $foo?->bar();
}

?>
--EXPECTF--
Fatal error: Cannot take reference of a nullsafe chain in %s on line %d

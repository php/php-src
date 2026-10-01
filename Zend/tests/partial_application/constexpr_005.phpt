--TEST--
PFA in constexpr: variadic placeholder must be after positional params
--FILE--
<?php

function f($a = g(..., 1)) {
}

f();

?>
--EXPECTF--
Fatal error: Variadic placeholder must be last in %s on line %d

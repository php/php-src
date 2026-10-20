--TEST--
PFA in constexpr: variadic placeholder may only appear once
--FILE--
<?php

function f($a = g(..., ...)) {
}

f();

?>
--EXPECTF--
Fatal error: Variadic placeholder may only appear once in %s on line %d

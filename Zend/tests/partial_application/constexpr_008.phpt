--TEST--
PFA in constexpr: variadic placeholder not allowed to be named
--FILE--
<?php

function f($a = g(foo: ...)) {
}

f();

?>
--EXPECTF--
Parse error: syntax error, unexpected token "..." in %s on line %d

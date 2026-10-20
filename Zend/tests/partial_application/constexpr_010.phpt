--TEST--
PFA in constexpr: error during partial creation
--FILE--
<?php

function f($a = g(?)) {
}

function g($a, $b) {
}

try {
    f();
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

?>
--EXPECT--
ArgumentCountError: Partial application of g() expects exactly 2 arguments, 1 given

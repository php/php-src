--TEST--
PFA in constexpr: error during arg list evaluation
--FILE--
<?php

function f($a = g(strlen(?), e: strlen(?), a: strlen(?), ...)) {
}

function g($a, $b, $c, $d, $e) {
}

function h($a = i(strlen(?), a: strlen(?), b: invalid(?), ...)) {
}

function i(...$args) {
}

function j($a = g(c: new stdClass, ...), $b = g(c: MISSING_CONST + 1, ...)) {
}

try {
    f();
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

try {
    h();
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

try {
    j();
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Named parameter $a overwrites previous argument
Error: Call to undefined function invalid()
Error: Undefined constant "MISSING_CONST"

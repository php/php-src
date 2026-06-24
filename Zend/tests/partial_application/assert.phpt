--TEST--
PFA of assert() behaves like a dynamic call to assert()
--FILE--
<?php

try {
    echo "# Static call:\n";
    assert(false);
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

try {
    echo "# Dynamic call:\n";
    (function ($f) { $f(false); })('assert');
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage() ?: '(no message)', "\n";
}

try {
    echo "# PFA call:\n";
    $f = assert(?);
    $f(false);
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage() ?: '(no message)', "\n";
}

?>
--EXPECT--
# Static call:
AssertionError: assert(false)
# Dynamic call:
AssertionError: (no message)
# PFA call:
AssertionError: (no message)

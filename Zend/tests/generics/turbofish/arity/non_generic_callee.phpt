--TEST--
Turbofish arity: turbofish on a non-generic callee throws ArgumentCountError
--FILE--
<?php
function f($x) { return $x; }
try {
    f::<int>(42);
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Too many generic type arguments to f(), 1 passed and exactly 0 expected

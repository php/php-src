--TEST--
Turbofish arity: too few type arguments to a function throws ArgumentCountError
--FILE--
<?php
function f<T, U>($x, $y) { return [$x, $y]; }
try {
    f::<int>(1, 2);
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Too few generic type arguments to f(), 1 passed and exactly 2 expected

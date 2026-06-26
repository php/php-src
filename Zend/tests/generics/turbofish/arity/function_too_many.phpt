--TEST--
Turbofish arity: too many type arguments to a function throws ArgumentCountError
--FILE--
<?php
function f<T>($x) { return $x; }
try {
    f::<int, string>(1);
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Too many generic type arguments to f(), 2 passed and exactly 1 expected

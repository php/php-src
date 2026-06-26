--TEST--
Turbofish: arity must match callee's declared generic parameters
--FILE--
<?php
function f<T>($x) { return $x; }
try {
    f::<int, string, float>(7);
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Too many generic type arguments to f(), 3 passed and exactly 1 expected

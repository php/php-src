--TEST--
Turbofish arity: providing fewer than required even with some defaults still throws
--FILE--
<?php
function f<A, B, C = mixed>($x) { return $x; }
try {
    f::<int>(1);
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Too few generic type arguments to f(), 1 passed and at least 2 expected

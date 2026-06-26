--TEST--
Turbofish arity: defaults extend the accepted arity range
--FILE--
<?php
function f<A, B = mixed, C = mixed>($x) { return $x; }

// arity 1, 2, 3 all valid
var_dump(f::<int>(1));
var_dump(f::<int, string>(2));
var_dump(f::<int, string, float>(3));

// arity 0 (i.e., no turbofish) is also valid
var_dump(f(4));

// arity 4 exceeds total
try {
    f::<int, int, int, int>(5);
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(1)
int(2)
int(3)
int(4)
Too many generic type arguments to f(), 4 passed and at most 3 expected

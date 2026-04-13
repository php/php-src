--TEST--
Generic function: return type enforcement
--DESCRIPTION--
Generic functions infer T from arguments and enforce return type at runtime.
If a generic function declares return type T but returns a value of a different
type than what T was inferred to be, a TypeError is thrown.
Note: PHP's standard weak type coercion still applies (e.g., int→string is OK
in weak mode), so tests use non-coercible type mismatches.
--FILE--
<?php

// 1. Basic: return type matches T — works fine
function identity<T>(T $value): T {
    return $value;
}

echo identity(42) . "\n";
echo identity("hello") . "\n";

// 2. Function that returns wrong type (array instead of string — not coercible)
function broken_identity<T>(T $value): T {
    return [1, 2, 3]; // returns array regardless of T
}

try {
    $result = broken_identity("hello"); // T=string inferred, but returns array
    echo "BUG: no TypeError for wrong return type\n";
} catch (TypeError $e) {
    echo "OK: caught TypeError\n";
}

// 3. Nested calls — return type should propagate through call chain
function wrap_and_return<T>(T $value): T {
    return identity($value);
}

echo wrap_and_return(42) . "\n";
echo wrap_and_return("test") . "\n";

// 4. Multiple type params — each should be checked independently
function swap_first<A, B>(A $a, B $b): A {
    return $b; // wrong! returns B(array) instead of A(string)
}

try {
    $result = swap_first("hello", [1, 2]); // A=string, B=array, returns array
    echo "BUG: no TypeError for returning B where A expected\n";
} catch (TypeError $e) {
    echo "OK: caught TypeError\n";
}

// 5. Parameter type checking DOES work (verify it)
function strict_param<T>(T $a, T $b): T {
    return $a;
}

echo strict_param(1, 2) . "\n";
echo strict_param("a", "b") . "\n";
?>
--EXPECT--
42
hello
OK: caught TypeError
42
test
OK: caught TypeError
1
a

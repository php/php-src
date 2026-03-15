--TEST--
Generic function: inference failure cases (all-or-nothing semantics)
--DESCRIPTION--
Tests that if not all generic type params can be inferred from call
arguments, the inference fails entirely and types go unchecked.
--FILE--
<?php

// 1. T only in return — can't infer, no enforcement
function make_value<T>(): T {
    return "hello";
}

// No TypeError even though T can't be inferred
echo "1. " . make_value() . "\n";

// 2. Two params, only one inferrable
function partial<A, B>(A $a): B {
    return $a; // B can't be inferred — no enforcement
}

// No TypeError — B is not in any parameter
echo "2. " . partial(42) . "\n";

// 3. No args at all, but T in params (called with fewer args than declared)
// This would be a regular PHP error, not a generics error
function needs_arg<T>(T $v): T {
    return $v;
}

try {
    needs_arg(); // Too few arguments
} catch (\ArgumentCountError $e) {
    echo "3. ArgumentCountError OK\n";
}

// 4. First param non-generic, second generic — inference works
function second_generic<T>(int $a, T $b): T {
    return $b;
}

echo "4. " . second_generic(1, "hello") . "\n";

// With wrong return:
function bad_second<T>(int $a, T $b): T {
    return [1]; // array, not T
}

try {
    bad_second(1, "hello"); // T=string, returning array
    echo "FAIL\n";
} catch (TypeError $e) {
    echo "4. TypeError OK\n";
}

echo "Done.\n";
?>
--EXPECT--
1. hello
2. 42
3. ArgumentCountError OK
4. hello
4. TypeError OK
Done.

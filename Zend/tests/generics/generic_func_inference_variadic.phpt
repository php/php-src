--TEST--
Generic function: inference from variadic parameters
--DESCRIPTION--
Tests that generic type params can be inferred from variadic arguments.
The variadic args are relocated by zend_copy_extra_args() to
EX_VAR_NUM(last_var + T), so inference reads from there.
--FILE--
<?php

// 1. Basic variadic inference
function first<T>(T ...$args): T {
    return $args[0];
}

echo "1. " . first(1, 2, 3) . "\n";
echo "1. " . first("a", "b") . "\n";

// 2. Wrong return type with variadic (array can't coerce to int)
function bad_first<T>(T ...$args): T {
    return [999];
}

try {
    bad_first(42, 43);
    echo "FAIL\n";
} catch (TypeError $e) {
    echo "2. TypeError OK\n";
}

// 3. Mixed regular + variadic params
function mixed_params<A, B>(A $first, B ...$rest): A {
    return $first;
}

echo "3. " . mixed_params("hello", 1, 2, 3) . "\n";

// Wrong return: should return A(string), not B(int)
function bad_mixed<A, B>(A $first, B ...$rest): A {
    return $rest[0]; // returns B instead of A
}

try {
    bad_mixed("hello", [1]); // A=string, B=array
    echo "FAIL\n";
} catch (TypeError $e) {
    echo "3. TypeError OK\n";
}

// 4. Variadic with no extra args — inference from regular param position
function variadic_empty<T>(T ...$args): T {
    return $args[0] ?? throw new RuntimeException("no args");
}

echo "4. " . variadic_empty(42) . "\n";

// 5. Single variadic arg
echo "5. " . first(3.14) . "\n";

echo "Done.\n";
?>
--EXPECT--
1. 1
1. a
2. TypeError OK
3. hello
3. TypeError OK
4. 42
5. 3.14
Done.

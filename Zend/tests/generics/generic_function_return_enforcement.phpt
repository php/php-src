--TEST--
Generic function: comprehensive return type enforcement
--DESCRIPTION--
Tests various scenarios for generic function return type enforcement via
lazy inference from call arguments. Uses non-coercible type mismatches
to avoid PHP's weak type coercion (int→string is allowed in weak mode).
--FILE--
<?php

// 1. Wrong return type throws TypeError (array is not coercible to string)
function broken<T>(T $v): T {
    return [1, 2, 3];
}

try {
    broken("hello");
    echo "FAIL: expected TypeError\n";
} catch (TypeError $e) {
    echo "1. OK: caught TypeError\n";
}

// 2. Correct return type passes
function identity<T>(T $v): T {
    return $v;
}

echo "2. OK: " . identity(42) . "\n";
echo "2. OK: " . identity("hello") . "\n";

// 3. Multiple type params — returning wrong param type fails
function swap<A, B>(A $a, B $b): A {
    return $b; // returns B(array) instead of A(string)
}

try {
    swap("hello", [1]);
    echo "FAIL: expected TypeError\n";
} catch (TypeError $e) {
    echo "3. OK: caught TypeError\n";
}

// 4. Type param only in return (not in params) — can't infer, remains unchecked
function make_int<T>(): T {
    return 42;
}

echo "4. OK: " . make_int() . "\n"; // no TypeError — T can't be inferred

// 5. Nested calls — each function infers independently
function outer<T>(T $v): T {
    return identity($v); // identity infers its own T
}

echo "5. OK: " . outer(99) . "\n";
echo "5. OK: " . outer("nested") . "\n";

// 6. Variadic — infer T from first variadic arg
function first<T>(T ...$args): T {
    return $args[0];
}

echo "6. OK: " . first(1, 2, 3) . "\n";
echo "6. OK: " . first("a", "b") . "\n";

// 7. Variadic with wrong return type (array can't coerce to string)
function bad_first<T>(T ...$args): T {
    return [3.14];
}

try {
    bad_first("a", "b");
    echo "FAIL: expected TypeError\n";
} catch (TypeError $e) {
    echo "7. OK: caught TypeError\n";
}

// 8. Correct return of same type
function passthrough<T>(T $v): T {
    return $v;
}

echo "8. OK: " . passthrough(true) . "\n";

// 9. Int return for int param — no error
function double_it<T>(T $v): T {
    if (is_int($v)) return $v * 2;
    return $v;
}

echo "9. OK: " . double_it(5) . "\n";
echo "9. OK: " . double_it("keep") . "\n";

// 10. Coercible types are allowed in weak mode (int→string is fine)
function weak_coerce<T>(T $v): T {
    return 42; // int can coerce to string in weak mode
}

echo "10. OK: " . weak_coerce("hello") . "\n"; // passes due to int→string coercion

?>
--EXPECT--
1. OK: caught TypeError
2. OK: 42
2. OK: hello
3. OK: caught TypeError
4. OK: 42
5. OK: 99
5. OK: nested
6. OK: 1
6. OK: a
7. OK: caught TypeError
8. OK: 1
9. OK: 10
9. OK: keep
10. OK: 42

--TEST--
Generic function return type enforcement with opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

// 1. Basic enforcement: wrong return type (non-coercible)
function broken<T>(T $v): T {
    return [1, 2];
}

try {
    broken("hello");
    echo "FAIL\n";
} catch (TypeError $e) {
    echo "1. OK: caught TypeError\n";
}

// 2. Correct return passes
function identity<T>(T $v): T {
    return $v;
}

echo "2. OK: " . identity(42) . "\n";
echo "2. OK: " . identity("hello") . "\n";

// 3. Multiple type params
function swap<A, B>(A $a, B $b): A {
    return $b;
}

try {
    swap("hello", [1]);
    echo "FAIL\n";
} catch (TypeError $e) {
    echo "3. OK: caught TypeError\n";
}

// 4. Uninferrable type param remains unchecked
function make<T>(): T {
    return 42;
}

echo "4. OK: " . make() . "\n";

// 5. Hot loop to trigger optimizer passes
function add_one<T>(T $v): T {
    return $v;
}

for ($i = 0; $i < 100; $i++) {
    add_one($i);
}
echo "5. OK: hot loop\n";

// 6. Variadic
function first<T>(T ...$args): T {
    return $args[0];
}

echo "6. OK: " . first("a", "b", "c") . "\n";

// 7. Variadic with wrong return
function bad_variadic<T>(T ...$args): T {
    return [1];
}

try {
    bad_variadic("a", "b");
    echo "FAIL\n";
} catch (TypeError $e) {
    echo "7. OK: caught TypeError\n";
}

echo "Done.\n";
?>
--EXPECT--
1. OK: caught TypeError
2. OK: 42
2. OK: hello
3. OK: caught TypeError
4. OK: 42
5. OK: hot loop
6. OK: a
7. OK: caught TypeError
Done.

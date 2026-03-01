--TEST--
Generic function return type enforcement with JIT
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=1255
opcache.jit_buffer_size=64M
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

// 4. Hot loop — triggers JIT compilation of generic function
function increment<T>(T $v): T {
    return $v;
}

for ($i = 0; $i < 200; $i++) {
    increment($i);
}
echo "4. OK: hot loop int\n";

for ($i = 0; $i < 200; $i++) {
    increment("str_$i");
}
echo "4. OK: hot loop string\n";

// 5. Hot loop with enforcement check after JIT
function wrong_after_jit<T>(T $v): T {
    return [999];
}

// First make it hot with a type that fails non-coercibly
$caught = 0;
for ($i = 0; $i < 200; $i++) {
    try {
        wrong_after_jit("test");
    } catch (TypeError $e) {
        $caught++;
    }
}
echo "5. OK: caught $caught TypeErrors in hot loop\n";

// 6. Variadic with JIT
function first<T>(T ...$args): T {
    return $args[0];
}

for ($i = 0; $i < 200; $i++) {
    first($i, $i + 1, $i + 2);
}
echo "6. OK: variadic hot loop\n";

// 7. Nested generic calls under JIT
function outer<T>(T $v): T {
    return identity($v);
}

for ($i = 0; $i < 200; $i++) {
    outer($i);
}
echo "7. OK: nested hot loop\n";

echo "Done.\n";
?>
--EXPECT--
1. OK: caught TypeError
2. OK: 42
2. OK: hello
3. OK: caught TypeError
4. OK: hot loop int
4. OK: hot loop string
5. OK: caught 200 TypeErrors in hot loop
6. OK: variadic hot loop
7. OK: nested hot loop
Done.

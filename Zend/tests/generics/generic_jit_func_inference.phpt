--TEST--
Generic function: JIT + function type inference
--DESCRIPTION--
Tests that generic function argument inference and return type
enforcement work correctly under JIT compilation.
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.jit_hot_func=1
--FILE--
<?php

// 1. Basic: identity function, correct return
function identity<T>(T $v): T {
    return $v;
}

echo "1. " . identity(42) . "\n";
echo "1. " . identity("hello") . "\n";

// 2. Wrong return type (array is not coercible to string)
function broken<T>(T $v): T {
    return [1, 2];
}

try {
    broken("test");
    echo "FAIL\n";
} catch (TypeError $e) {
    echo "2. TypeError OK\n";
}

// 3. Hot loop — trigger JIT compilation
for ($i = 0; $i < 200; $i++) {
    identity($i);
}
echo "3. hot loop int OK\n";

for ($i = 0; $i < 200; $i++) {
    identity("str_$i");
}
echo "3. hot loop string OK\n";

// 4. After JIT, enforcement must still work
try {
    broken("after-jit");
    echo "FAIL\n";
} catch (TypeError $e) {
    echo "4. post-jit TypeError OK\n";
}

// 5. Multiple type params under JIT
function pair<A, B>(A $a, B $b): A {
    return $a;
}

for ($i = 0; $i < 200; $i++) {
    pair($i, "x");
}
echo "5. multi-param hot OK\n";

// Wrong return for multi-param
function bad_pair<A, B>(A $a, B $b): A {
    return $b; // returns B instead of A
}

try {
    bad_pair("hello", [1]); // A=string, B=array, returning array
    echo "FAIL\n";
} catch (TypeError $e) {
    echo "5. multi-param TypeError OK\n";
}

echo "Done.\n";
?>
--EXPECT--
1. 42
1. hello
2. TypeError OK
3. hot loop int OK
3. hot loop string OK
4. post-jit TypeError OK
5. multi-param hot OK
5. multi-param TypeError OK
Done.

--TEST--
GH-25379: array_map() foreach optimization callee single evaluation and memory leaks on fallback
--FILE--
<?php

class ConstHolder {
    public const int VAL = 42;
}

const CONST_INT = 10;

function getf(): string {
    echo "getf\n";
    return str_repeat('strto', 1) . 'upper';
}

function getc(): object {
    echo "getc\n";
    return new class {
        public static function m($x) {
            return $x;
        }
    };
}

function getm(): string {
    echo "getm\n";
    return str_repeat('m', 1);
}

function throw_getm(): string {
    throw new RuntimeException("boom m");
}

function fn_named(int $a, int $b): int {
    return $a + $b;
}

function getf_named(): string {
    echo "getf_named\n";
    return str_repeat('fn_', 1) . 'named';
}

/* 1. Non-literal callee with IS_CONST int */
try {
    var_dump(array_map(getf()(...), 5));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* 2. Non-literal callee with IS_CONST string */
try {
    array_map(getf()(...), "hello");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* 3. Non-literal callee with IS_CONST null */
try {
    array_map(getf()(...), null);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* 4. Non-literal callee with IS_CONST bool */
try {
    array_map(getf()(...), false);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* 5. Non-literal class with IS_CONST int */
try {
    array_map(getc()::m(...), 5);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* 6. Non-literal class and method with IS_CONST int */
try {
    array_map(getc()::{(getm())}(...), 5);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* 7. Non-existent class with dynamic method and IS_CONST int */
try {
    array_map(Foo::{(getm())}(...), 5);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

/* 8. Exception in dynamic method evaluation */
try {
    array_map(getc()::{(throw_getm())}(...), 5);
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}

/* 9. Global constant non-array */
try {
    array_map(getf()(...), CONST_INT);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* 10. Class constant non-array */
try {
    array_map(getf()(...), ConstHolder::VAL);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* 11. Constant binary expression non-array */
try {
    array_map(getf()(...), 1 + 2);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* 12. Variable callable with IS_CONST int */
$f = 'strtoupper';
try {
    array_map($f(...), 5);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* 13. Non-literal callee with __FILE__ */
try {
    array_map(getf()(...), __FILE__);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* 14. Non-literal callee with stdClass::class */
try {
    array_map(getf()(...), stdClass::class);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* 15. Non-literal callee with true */
try {
    array_map(getf()(...), true);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* 16. Non-literal callee with __FILE__ . '.php' */
try {
    array_map(getf()(...), __FILE__ . '.php');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* 17. PFA with multiple placeholders falls back, callee evaluated once */
try {
    var_dump(array_map(getf()(?, ?), [1]));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

/* 18. PFA with variadic placeholder and named args falls back, callee evaluated once */
try {
    var_dump(array_map(getf_named()(a: 1, ...), [2]));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

/* 19. PFA with assert and \assert falls back cleanly */
var_dump(array_map(assert(...), [1]));
var_dump(array_map(\assert(...), [1]));

/* 20. Non-interned IS_CONST string produced during expression compilation */
try {
    array_map(getf()(...), "unique_non_interned_" . strlen("abcdef"));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* 21. IS_CONST int produced during expression compilation */
try {
    array_map(getf()(...), strlen("abcdef"));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* 22. Dynamic static call with non-interned IS_CONST string */
try {
    array_map(getc()::m(...), "unique_non_interned_static_" . strlen("abcdef"));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* 23. Exception in dynamic method evaluation on optimized path */
try {
    array_map(getc()::{(throw_getm())}(...), "unique_non_interned_throw_" . strlen("abcdef"));
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
getf
array_map(): Argument #2 ($array) must be of type array, int given
getf
array_map(): Argument #2 ($array) must be of type array, string given
getf
array_map(): Argument #2 ($array) must be of type array, null given
getf
array_map(): Argument #2 ($array) must be of type array, false given
getc
array_map(): Argument #2 ($array) must be of type array, int given
getc
getm
array_map(): Argument #2 ($array) must be of type array, int given
getm
Class "Foo" not found
getc
boom m
getf
array_map(): Argument #2 ($array) must be of type array, int given
getf
array_map(): Argument #2 ($array) must be of type array, int given
getf
array_map(): Argument #2 ($array) must be of type array, int given
array_map(): Argument #2 ($array) must be of type array, int given
getf
array_map(): Argument #2 ($array) must be of type array, string given
getf
array_map(): Argument #2 ($array) must be of type array, string given
getf
array_map(): Argument #2 ($array) must be of type array, true given
getf
array_map(): Argument #2 ($array) must be of type array, string given
getf
Partial application of strtoupper() expects at most 1 arguments, 2 given
getf_named
array(1) {
  [0]=>
  int(3)
}
array(1) {
  [0]=>
  bool(true)
}
array(1) {
  [0]=>
  bool(true)
}
getf
array_map(): Argument #2 ($array) must be of type array, string given
getf
array_map(): Argument #2 ($array) must be of type array, int given
getc
array_map(): Argument #2 ($array) must be of type array, string given
getc
boom m

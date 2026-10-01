--TEST--
No coercion should be applied to type true
--FILE--
<?php

function test(true $v) { var_dump($v); }

try {
    test(1);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    test('1');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    test([1]);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    test(new stdClass());
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
TypeError: test(): Argument #1 ($v) must be of type true, int given, called in %s on line %d
TypeError: test(): Argument #1 ($v) must be of type true, string given, called in %s on line %d
TypeError: test(): Argument #1 ($v) must be of type true, array given, called in %s on line %d
TypeError: test(): Argument #1 ($v) must be of type true, stdClass given, called in %s on line %d

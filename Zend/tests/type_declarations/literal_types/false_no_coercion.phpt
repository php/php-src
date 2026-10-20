--TEST--
No coercion should be applied to type false
--FILE--
<?php

function test(false $v) { var_dump($v); }

try {
    test(0);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    test('');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    test([]);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
TypeError: test(): Argument #1 ($v) must be of type false, int given, called in %s on line %d
TypeError: test(): Argument #1 ($v) must be of type false, string given, called in %s on line %d
TypeError: test(): Argument #1 ($v) must be of type false, array given, called in %s on line %d

--TEST--
No coercion should be applied to type false
--FILE--
<?php

function test(false $v) { var_dump($v); }

try {
    test(0);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    test('');
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    test([]);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

?>
--EXPECTF--
test(): Argument #1 ($v) must be of type false, int given, called in %s on line %d
test(): Argument #1 ($v) must be of type false, string given, called in %s on line %d
test(): Argument #1 ($v) must be of type false, array given, called in %s on line %d

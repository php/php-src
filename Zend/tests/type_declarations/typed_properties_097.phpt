--TEST--
Incrementing/decrementing past max/min value (additional cases)
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die('skip 64 bit test'); ?>
--FILE--
<?php

class Test {
    public int $foo;
}

$test = new Test;

$test->foo = PHP_INT_MIN;
try {
    --$test->foo;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($test->foo);
try {
    $test->foo--;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($test->foo);

$test->foo = PHP_INT_MAX;
try {
    ++$test->foo;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($test->foo);
try {
    $test->foo++;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($test->foo);

// Do the same things again, but with the property being a reference.
$ref =& $test->foo;

$test->foo = PHP_INT_MIN;
try {
    --$test->foo;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($test->foo);
try {
    $test->foo--;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($test->foo);

$test->foo = PHP_INT_MAX;
try {
    ++$test->foo;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($test->foo);
try {
    $test->foo++;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($test->foo);

?>
--EXPECT--
TypeError: Cannot decrement property Test::$foo of type int past its minimal value
int(-9223372036854775808)
TypeError: Cannot decrement property Test::$foo of type int past its minimal value
int(-9223372036854775808)
TypeError: Cannot increment property Test::$foo of type int past its maximal value
int(9223372036854775807)
TypeError: Cannot increment property Test::$foo of type int past its maximal value
int(9223372036854775807)
TypeError: Cannot decrement a reference held by property Test::$foo of type int past its minimal value
int(-9223372036854775808)
TypeError: Cannot decrement a reference held by property Test::$foo of type int past its minimal value
int(-9223372036854775808)
TypeError: Cannot increment a reference held by property Test::$foo of type int past its maximal value
int(9223372036854775807)
TypeError: Cannot increment a reference held by property Test::$foo of type int past its maximal value
int(9223372036854775807)

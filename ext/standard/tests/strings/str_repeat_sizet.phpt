--TEST--
str_repeat(): $times must fit into size_t when zend_long is wider
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip this test is for PHP_SYS_SIZE < PHP_INT_SIZE only");
}
?>
--FILE--
<?php
/* zend_string_safe_alloc() takes size_t. Where zend_long is wider than size_t,
 * $times has to be rejected before it is narrowed, because the overflow check
 * inside the allocator would only ever see the truncated value. Anything that
 * does fit size_t is left to that check and to the allocator, exactly as on a
 * build where the two types are the same width. */

$sizeMax = 2 ** (PHP_SYS_SIZE * 8) - 1;   // SIZE_MAX

echo "-- wider than size_t: rejected before narrowing --\n";
foreach ([$sizeMax + 1, $sizeMax * 2, PHP_INT_MAX] as $times) {
    try {
        var_dump(str_repeat('12', $times));
    } catch (ValueError $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

echo "-- negative still reports the plain message --\n";
try {
    var_dump(str_repeat('12', -1));
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "-- small enough to be a valid size_t: not rejected, allocation decides --\n";
var_dump(str_repeat('1', $sizeMax));
?>
--EXPECTF--
-- wider than size_t: rejected before narrowing --
ValueError: str_repeat(): Argument #2 ($times) must be less than or equal to %d
ValueError: str_repeat(): Argument #2 ($times) must be less than or equal to %d
ValueError: str_repeat(): Argument #2 ($times) must be less than or equal to %d
-- negative still reports the plain message --
ValueError: str_repeat(): Argument #2 ($times) must be greater than or equal to 0
-- small enough to be a valid size_t: not rejected, allocation decides --

Fatal error: Possible integer overflow in memory allocation (%s) in %s on line %d

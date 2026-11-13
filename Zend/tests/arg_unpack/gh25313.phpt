--TEST--
GH-25313 (Assertion !EG(exception) in ZEND_FE_FREE when unpacking a generator into a by-ref parameter throws)
--CREDITS--
YuanchengJiang
--FILE--
<?php
set_error_handler(function($n, $s) { throw new Exception($s); });

function test($a, $b, $c, &$ref) {}

function gen($array) {
    foreach ($array as $e)
        yield $e;
}

try {
    test(...gen([1, 2]), ...gen([3, 4]));
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

try {
    test(...gen(['a' => 1, 'b' => 2]), ...gen(['c' => 3, 'ref' => 4]));
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: Cannot pass by-reference argument 4 of test() by unpacking a Traversable, passing by-value instead
Exception: Cannot pass by-reference argument 4 of test() by unpacking a Traversable, passing by-value instead

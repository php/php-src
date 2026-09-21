--TEST--
SplFixedArray::fromArray() index overflow on narrow size_t
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip size_t is not narrower than zend_long on this platform");
}
?>
--FILE--
<?php
/* With $preserveKeys the size comes from the largest index plus one, which is a
 * zend_long taken straight from the array. Only the signed wrap to <= 0 is
 * checked, so a size above SIZE_MAX still reaches safe_emalloc() and is narrowed
 * there, leaving a huge reported size over a small buffer that the following
 * element copy writes past. */
$ssizeMax = 2 ** (PHP_SYS_SIZE * 8 - 1) - 1;

/* An index of N asks for a size of N + 1. */
$indexes = [
    'SSIZE_MAX'   => $ssizeMax,
    'SSIZE_MAX+1' => $ssizeMax + 1,
    'SIZE_MAX'    => 2 ** (PHP_SYS_SIZE * 8) - 1,
    'INT_MAX-1'   => PHP_INT_MAX - 1,
];

foreach ($indexes as $label => $index) {
    echo "$label: ";
    try {
        $a = SplFixedArray::fromArray([$index => 'x'], true);
        echo "unexpected success; getSize() = ", $a->getSize(), "\n";
    } catch (Error|Exception $e) {
        echo $e::class . ': ' . $e->getMessage(), "\n";
    }
}

/* An index whose size still fits ssize_t must not be refused here: it reaches
 * safe_emalloc(), which rejects it because size * sizeof(zval) overflows. */
SplFixedArray::fromArray([$ssizeMax - 1 => 'x'], true);
?>
--EXPECTF--
SSIZE_MAX: InvalidArgumentException: integer overflow detected
SSIZE_MAX+1: InvalidArgumentException: integer overflow detected
SIZE_MAX: InvalidArgumentException: integer overflow detected
INT_MAX-1: InvalidArgumentException: integer overflow detected

Fatal error: Possible integer overflow in memory allocation (%d * %d + 0) in %s on line %d

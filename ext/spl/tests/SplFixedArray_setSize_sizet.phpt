--TEST--
SplFixedArray::setSize() $size overflow on narrow size_t
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip size_t is not narrower than zend_long on this platform");
}
?>
--FILE--
<?php
/* spl_fixedarray_resize() grows the array with safe_erealloc(), whose nmemb is a
 * size_t, so a size above SIZE_MAX is narrowed before the allocator sees it. The
 * element initialisation that follows then computes elements + size, which wraps
 * as well and so writes nothing, leaving an object that reports a huge size over
 * a tiny buffer: every later index passes the bounds check. */
$ssizeMax = 2 ** (PHP_SYS_SIZE * 8 - 1) - 1;

$sizes = [
    'SSIZE_MAX+1' => $ssizeMax + 1,
    'SIZE_MAX'    => 2 ** (PHP_SYS_SIZE * 8) - 1,
    '2^61+1'      => 0x2000000000000001,
    'INT_MAX'     => PHP_INT_MAX,
];

foreach ($sizes as $label => $size) {
    echo "$label: ";
    $a = new SplFixedArray(1);
    try {
        $a->setSize($size);
        echo "unexpected success; getSize() = ", $a->getSize(), "\n";
    } catch (Error $e) {
        echo $e::class . ': ' . $e->getMessage(), "; getSize() = ", $a->getSize(), "\n";
    }
}

/* A size that does fit ssize_t must not be refused here: it reaches
 * safe_erealloc(), which rejects it because size * sizeof(zval) overflows. */
$a = new SplFixedArray(1);
$a->setSize($ssizeMax);
?>
--EXPECTF--
SSIZE_MAX+1: ValueError: SplFixedArray::setSize(): Argument #1 ($size) must be less than or equal to %d; getSize() = 1
SIZE_MAX: ValueError: SplFixedArray::setSize(): Argument #1 ($size) must be less than or equal to %d; getSize() = 1
2^61+1: ValueError: SplFixedArray::setSize(): Argument #1 ($size) must be less than or equal to %d; getSize() = 1
INT_MAX: ValueError: SplFixedArray::setSize(): Argument #1 ($size) must be less than or equal to %d; getSize() = 1

Fatal error: Possible integer overflow in memory allocation (%d * %d + 0) in %s on line %d

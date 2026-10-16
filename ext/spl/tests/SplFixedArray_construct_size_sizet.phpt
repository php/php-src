--TEST--
SplFixedArray::__construct() $size overflow on narrow size_t
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip size_t is not narrower than zend_long on this platform");
}
?>
--FILE--
<?php
/* spl_fixedarray_init_non_empty_struct() passes the size straight to
 * safe_emalloc(), whose nmemb is a size_t. A size above SIZE_MAX is narrowed on
 * the way in, so the allocator checks a value that is not the one asked for and
 * allocates a buffer far too small while the object records the full size. Every
 * such size has to be refused before the narrowing. */
$ssizeMax = 2 ** (PHP_SYS_SIZE * 8 - 1) - 1;

$sizes = [
    'SSIZE_MAX+1' => $ssizeMax + 1,
    'SIZE_MAX'    => 2 ** (PHP_SYS_SIZE * 8) - 1,
    '2^61+1'      => 0x2000000000000001,
    'INT_MAX'     => PHP_INT_MAX,
];

foreach ($sizes as $label => $size) {
    echo "$label: ";
    try {
        $a = new SplFixedArray($size);
        echo "unexpected success; getSize() = ", $a->getSize(), "\n";
    } catch (Error $e) {
        echo $e::class . ': ' . $e->getMessage(), "\n";
    }
}

/* A size that does fit ssize_t must not be refused here: it reaches
 * safe_emalloc(), which rejects it because size * sizeof(zval) overflows. */
new SplFixedArray($ssizeMax);
?>
--EXPECTF--
SSIZE_MAX+1: ValueError: SplFixedArray::__construct(): Argument #1 ($size) must be less than or equal to %d
SIZE_MAX: ValueError: SplFixedArray::__construct(): Argument #1 ($size) must be less than or equal to %d
2^61+1: ValueError: SplFixedArray::__construct(): Argument #1 ($size) must be less than or equal to %d
INT_MAX: ValueError: SplFixedArray::__construct(): Argument #1 ($size) must be less than or equal to %d

Fatal error: Possible integer overflow in memory allocation (%d * %d + 0) in %s on line %d

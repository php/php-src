--TEST--
Assigning to a string offset that would exceed the maximum string length
--INI--
memory_limit=128M
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip size_t is not narrower than zend_long on this platform");
}
?>
--FILE--
<?php
/* Assigning to offset N extends the string to N + 1 bytes, so the largest
 * usable offset is PHP_STRING_MAX_LENGTH - 1. Above that the length is
 * narrowed to size_t on its way into zend_string_extend(): offsets beyond
 * SIZE_MAX alias down to a small index and the assignment silently succeeds
 * at the wrong place, while offsets just below it wrap the allocation to a
 * tiny buffer carrying a huge ZSTR_LEN which the gap fill then memset()s past
 * the end of. Every one of them has to be refused with the string left
 * untouched. */
$sizeMax = 2 ** (PHP_SYS_SIZE * 8) - 1;

$offsets = [
    'SIZE_MAX+11' => $sizeMax + 11,
    'STR_MAX'     => PHP_STRING_MAX_LENGTH,
    'STR_MAX+1'   => PHP_STRING_MAX_LENGTH + 1,
    'STR_MAX+2'   => PHP_STRING_MAX_LENGTH + 2,
    'SIZE_MAX-2'  => $sizeMax - 2,
    'SIZE_MAX-1'  => $sizeMax - 1,
    'SIZE_MAX'    => $sizeMax,
    'INT_MAX'     => PHP_INT_MAX,
];

foreach ($offsets as $label => $offset) {
    echo "$label: ";
    $s = "abc";
    try {
        $s[$offset] = 'x';
        $result = 'no error';
    } catch (Error $e) {
        $result = $e::class . ': ' . $e->getMessage();
    }
    echo $result, '; $s = ', var_export($s, true), "\n";
}

/* The largest usable offset must not be refused: it reaches the allocator and
 * fails there, as it would on a build where zend_long is no wider than size_t.
 *
 * Which diagnostic comes back depends on the build, so only the fact that it
 * got that far is asserted. ZSTR_MAX_LEN does not account for the per
 * allocation zend_mm_debug_info that a debug build adds, so at the very top of
 * the range a debug build reports the overflow of that addition ("Possible
 * integer overflow in memory allocation") while a release build has no such
 * overhead, attempts the allocation for real and reports the memory limit
 * ("Allowed memory size exhausted"). */
$s = "abc";
$s[PHP_STRING_MAX_LENGTH - 1] = 'x';
?>
--EXPECTF--
SIZE_MAX+11: Error: String size overflow; $s = 'abc'
STR_MAX: Error: String size overflow; $s = 'abc'
STR_MAX+1: Error: String size overflow; $s = 'abc'
STR_MAX+2: Error: String size overflow; $s = 'abc'
SIZE_MAX-2: Error: String size overflow; $s = 'abc'
SIZE_MAX-1: Error: String size overflow; $s = 'abc'
SIZE_MAX: Error: String size overflow; $s = 'abc'
INT_MAX: Error: String size overflow; $s = 'abc'

Fatal error: %s in %s on line %d

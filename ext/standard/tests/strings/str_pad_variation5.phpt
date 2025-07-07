--TEST--
Test str_pad() function : usage variations - unexpected large value for '$pad_length' argument
--INI--
memory_limit=128M
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip this test is for PHP_SYS_SIZE < PHP_INT_SIZE only");
}
if (getenv("USE_ZEND_ALLOC") === "0") {
    die("skip Zend MM disabled");
}
?>
--FILE--
<?php
$input = "Test string";

try {
    var_dump( str_pad($input, PHP_INT_MAX) );
} catch (ValueError $e) {
    echo $e::class . ": {$e->getMessage()}\n";
}

/* str_pad() passes the padding count to zend_string_safe_alloc() as its
 * unchecked third operand, which adds the header and the terminating NUL to
 * it. Any resulting length above PHP_STRING_MAX_LENGTH wraps to a tiny
 * allocation whose ZSTR_LEN is huge, and the padding is then memset() past
 * the end of it. The whole window from the limit up to SIZE_MAX has to be
 * refused, not just the values that exceed SIZE_MAX itself. */
$sizeMax = 2 ** (PHP_SYS_SIZE * 8) - 1;

$lengths = [
    'STR_MAX+1'  => PHP_STRING_MAX_LENGTH + 1,
    'STR_MAX+2'  => PHP_STRING_MAX_LENGTH + 2,
    'STR_MAX+3'  => PHP_STRING_MAX_LENGTH + 3,
    'SIZE_MAX-2' => $sizeMax - 2,
    'SIZE_MAX-1' => $sizeMax - 1,
    'SIZE_MAX'   => $sizeMax,
];

foreach ($lengths as $label => $length) {
    try {
        var_dump( str_pad($input, $length, '-') );
    } catch (ValueError $e) {
        echo "$label: {$e->getMessage()}\n";
    }
}

/* Below the limit the request is valid: it reaches the allocator and fails
 * there rather than being refused or narrowed. (PHP_STRING_MAX_LENGTH itself
 * is deliberately not used here: right at the top of the range the allocator
 * reports the wrap of its own alignment rounding instead of the memory limit,
 * and which of the two you get depends on the build.) */
var_dump( str_pad($input, (2 ** (PHP_SYS_SIZE * 8-2) - 1 << 1) + 1) );
?>
--EXPECTF--
ValueError: str_pad(): Argument #2 ($length) must be less than or equal to %d
STR_MAX+1: str_pad(): Argument #2 ($length) must be less than or equal to %d
STR_MAX+2: str_pad(): Argument #2 ($length) must be less than or equal to %d
STR_MAX+3: str_pad(): Argument #2 ($length) must be less than or equal to %d
SIZE_MAX-2: str_pad(): Argument #2 ($length) must be less than or equal to %d
SIZE_MAX-1: str_pad(): Argument #2 ($length) must be less than or equal to %d
SIZE_MAX: str_pad(): Argument #2 ($length) must be less than or equal to %d

Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d

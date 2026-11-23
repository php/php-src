--TEST--
FFI: array types whose size overflows size_t are rejected
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
/* The size of an array type was computed as length * element size without
 * an overflow check. A length that is valid on its own wrapped the size around
 * to a few bytes, so FFI::new() allocated a tiny buffer while element access,
 * which is only checked against the length, reached far beyond it. */
$bits = PHP_INT_SIZE * 8;
$max = (1 << ($bits - 2)) - 1;  // int32_t[$max] is the largest int32_t array that fits
$wrap = $max + 2;               // int32_t[$wrap] wraps around to a size of 4 bytes
$half = 1 << ($bits - 3);       // int32_t[$half] fits, int32_t[4][$half] doesn't
$quarter = $half >> 1;          // int32_t[3][$quarter] still fits

$cases = [
    'FFI::arrayType() just too large' =>
        fn() => FFI::arrayType(FFI::cdef()->type('int32_t'), [$max + 1]),
    'FFI::arrayType()' =>
        fn() => FFI::arrayType(FFI::cdef()->type('int32_t'), [$wrap]),
    'FFI::arrayType() of PHP_INT_MAX' =>
        fn() => FFI::arrayType(FFI::cdef()->type('int32_t'), [PHP_INT_MAX]),
    'FFI::arrayType() outer dimension' =>
        fn() => FFI::arrayType(FFI::cdef()->type('int32_t'), [4, $half]),
    'cdef array just too large' =>
        fn() => FFI::cdef("typedef int32_t t[" . ($max + 1) . "];"),
    'cdef array' =>
        fn() => FFI::cdef("typedef int32_t t[$wrap];"),
    'cdef outer dimension' =>
        fn() => FFI::cdef("typedef int32_t t[4][$half];"),
    'cdef nested declarator' =>
        fn() => FFI::cdef("typedef int32_t (t[$wrap]);"),
    'cdef nested declarator outer dimension' =>
        fn() => FFI::cdef("typedef int32_t (t[4][$half]);"),
];

foreach ($cases as $label => $fn) {
    try {
        $fn();
        echo "$label: unexpected success\n";
    } catch (FFI\Exception $e) {
        echo "$label: ", $e::class, ": ", $e->getMessage(), "\n";
    }
}

/* The largest arrays that still fit are accepted. */
FFI::arrayType(FFI::cdef()->type('int32_t'), [$max]);
FFI::arrayType(FFI::cdef()->type('int32_t'), [3, $quarter]);
FFI::cdef("typedef int32_t t1[$max]; typedef int32_t t2[3][$quarter];");
FFI::cdef("typedef int32_t (t1[$max]); typedef int32_t (t2[3][$quarter]);");

/* Arrays of zero-size elements (GNU C) stay zero-size whatever the length. */
$len = PHP_INT_MAX;
$ffi = FFI::cdef("struct e {}; struct z { int32_t a[0]; };");
var_dump(FFI::sizeof(FFI::arrayType($ffi->type('struct e'), [$len, $len])));
var_dump(FFI::sizeof(FFI::arrayType($ffi->type('struct z'), [$len])));
$ffi = FFI::cdef("struct e {}; typedef struct e t1[$len][$len]; typedef struct e (t2[$len]);");
var_dump(FFI::sizeof($ffi->type('t1')), FFI::sizeof($ffi->type('t2')));
echo "done\n";
?>
--EXPECT--
FFI::arrayType() just too large: FFI\Exception: array size is too large
FFI::arrayType(): FFI\Exception: array size is too large
FFI::arrayType() of PHP_INT_MAX: FFI\Exception: array size is too large
FFI::arrayType() outer dimension: FFI\Exception: array size is too large
cdef array just too large: FFI\ParserException: Array size is too large at line 1
cdef array: FFI\ParserException: Array size is too large at line 1
cdef outer dimension: FFI\ParserException: Array size is too large at line 1
cdef nested declarator: FFI\ParserException: Array size is too large at line 1
cdef nested declarator outer dimension: FFI\ParserException: Array size is too large at line 1
int(0)
int(0)
int(0)
int(0)
done

--TEST--
FFI::memcpy(), FFI::memcmp(), FFI::memset() and FFI::string() reject invalid sizes
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef();
$array = $ffi->new("char[4]");
$small = $ffi->new("char[2]");
$pointer = FFI::addr($array[0]);

/* Nothing limits the size when passing $pointer alone, so it only gets sizes not beyond $array */
$pointerSizes = ['0' => 0, '2' => 2, '3' => 3, '-1' => -1, 'PHP_INT_MIN' => PHP_INT_MIN];
$allSizes = $pointerSizes + ['PHP_INT_MAX' => PHP_INT_MAX];
$cases = [
    'FFI::memcpy($small, "abc", %s)' => [fn($size) => FFI::memcpy($small, "abc", $size), $allSizes],
    'FFI::memcpy($pointer, $small, %s)' => [fn($size) => FFI::memcpy($pointer, $small, $size), $allSizes],
    'FFI::memcmp($small, $array, %s)' => [fn($size) => FFI::memcmp($small, $array, $size), $allSizes],
    'FFI::memcmp($pointer, $small, %s)' => [fn($size) => FFI::memcmp($pointer, $small, $size), $allSizes],
    'FFI::memcmp($pointer, $pointer, %s)' => [fn($size) => FFI::memcmp($pointer, $pointer, $size), $pointerSizes],
    'FFI::memset($small, ord("x"), %s)' => [fn($size) => FFI::memset($small, ord("x"), $size), $allSizes],
    'FFI::memset($pointer, ord("x"), %s)' => [fn($size) => FFI::memset($pointer, ord("x"), $size), $pointerSizes],
    'FFI::string($small, %s)' => [fn($size) => FFI::string($small, $size), $allSizes],
    'FFI::string($pointer, %s)' => [fn($size) => FFI::string($pointer, $size), $pointerSizes],
];
foreach ($cases as $label => [$case, $caseSizes]) {
    foreach ($caseSizes as $sizeLabel => $size) {
        try {
            $result = var_export($case($size), true);
        } catch (Throwable $e) {
            $result = $e::class . ": " . $e->getMessage();
        }
        echo sprintf($label, $sizeLabel), ": ", $result, "\n";
    }
}
?>
--EXPECT--
FFI::memcpy($small, "abc", 0): NULL
FFI::memcpy($small, "abc", 2): NULL
FFI::memcpy($small, "abc", 3): FFI\Exception: Attempt to write over data boundary
FFI::memcpy($small, "abc", -1): ValueError: FFI::memcpy(): Argument #3 ($size) must be greater than or equal to 0
FFI::memcpy($small, "abc", PHP_INT_MIN): ValueError: FFI::memcpy(): Argument #3 ($size) must be greater than or equal to 0
FFI::memcpy($small, "abc", PHP_INT_MAX): FFI\Exception: Attempt to write over data boundary
FFI::memcpy($pointer, $small, 0): NULL
FFI::memcpy($pointer, $small, 2): NULL
FFI::memcpy($pointer, $small, 3): FFI\Exception: Attempt to read over data boundary
FFI::memcpy($pointer, $small, -1): ValueError: FFI::memcpy(): Argument #3 ($size) must be greater than or equal to 0
FFI::memcpy($pointer, $small, PHP_INT_MIN): ValueError: FFI::memcpy(): Argument #3 ($size) must be greater than or equal to 0
FFI::memcpy($pointer, $small, PHP_INT_MAX): FFI\Exception: Attempt to read over data boundary
FFI::memcmp($small, $array, 0): 0
FFI::memcmp($small, $array, 2): 0
FFI::memcmp($small, $array, 3): FFI\Exception: attempt to read over data boundary
FFI::memcmp($small, $array, -1): ValueError: FFI::memcmp(): Argument #3 ($size) must be greater than or equal to 0
FFI::memcmp($small, $array, PHP_INT_MIN): ValueError: FFI::memcmp(): Argument #3 ($size) must be greater than or equal to 0
FFI::memcmp($small, $array, PHP_INT_MAX): FFI\Exception: attempt to read over data boundary
FFI::memcmp($pointer, $small, 0): 0
FFI::memcmp($pointer, $small, 2): 0
FFI::memcmp($pointer, $small, 3): FFI\Exception: Attempt to read over data boundary
FFI::memcmp($pointer, $small, -1): ValueError: FFI::memcmp(): Argument #3 ($size) must be greater than or equal to 0
FFI::memcmp($pointer, $small, PHP_INT_MIN): ValueError: FFI::memcmp(): Argument #3 ($size) must be greater than or equal to 0
FFI::memcmp($pointer, $small, PHP_INT_MAX): FFI\Exception: Attempt to read over data boundary
FFI::memcmp($pointer, $pointer, 0): 0
FFI::memcmp($pointer, $pointer, 2): 0
FFI::memcmp($pointer, $pointer, 3): 0
FFI::memcmp($pointer, $pointer, -1): ValueError: FFI::memcmp(): Argument #3 ($size) must be greater than or equal to 0
FFI::memcmp($pointer, $pointer, PHP_INT_MIN): ValueError: FFI::memcmp(): Argument #3 ($size) must be greater than or equal to 0
FFI::memset($small, ord("x"), 0): NULL
FFI::memset($small, ord("x"), 2): NULL
FFI::memset($small, ord("x"), 3): FFI\Exception: attempt to write over data boundary
FFI::memset($small, ord("x"), -1): ValueError: FFI::memset(): Argument #3 ($size) must be greater than or equal to 0
FFI::memset($small, ord("x"), PHP_INT_MIN): ValueError: FFI::memset(): Argument #3 ($size) must be greater than or equal to 0
FFI::memset($small, ord("x"), PHP_INT_MAX): FFI\Exception: attempt to write over data boundary
FFI::memset($pointer, ord("x"), 0): NULL
FFI::memset($pointer, ord("x"), 2): NULL
FFI::memset($pointer, ord("x"), 3): NULL
FFI::memset($pointer, ord("x"), -1): ValueError: FFI::memset(): Argument #3 ($size) must be greater than or equal to 0
FFI::memset($pointer, ord("x"), PHP_INT_MIN): ValueError: FFI::memset(): Argument #3 ($size) must be greater than or equal to 0
FFI::string($small, 0): ''
FFI::string($small, 2): 'xx'
FFI::string($small, 3): FFI\Exception: attempt to read over data boundary
FFI::string($small, -1): ValueError: FFI::string(): Argument #2 ($size) must be greater than or equal to 0
FFI::string($small, PHP_INT_MIN): ValueError: FFI::string(): Argument #2 ($size) must be greater than or equal to 0
FFI::string($small, PHP_INT_MAX): FFI\Exception: attempt to read over data boundary
FFI::string($pointer, 0): ''
FFI::string($pointer, 2): 'xx'
FFI::string($pointer, 3): 'xxx'
FFI::string($pointer, -1): ValueError: FFI::string(): Argument #2 ($size) must be greater than or equal to 0
FFI::string($pointer, PHP_INT_MIN): ValueError: FFI::string(): Argument #2 ($size) must be greater than or equal to 0

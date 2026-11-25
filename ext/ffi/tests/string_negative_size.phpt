--TEST--
FFI::string() rejects a negative size
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef();
$array = $ffi->new("char[4]");
FFI::memcpy($array, "abcd", 4);
$pointer = FFI::addr($array[0]);

foreach (['$array' => $array, '$pointer' => $pointer] as $label => $cdata) {
    foreach (['3' => 3, '0' => 0, '-1' => -1, 'PHP_INT_MIN' => PHP_INT_MIN] as $sizeLabel => $size) {
        try {
            $result = var_export(FFI::string($cdata, $size), true);
            echo "FFI::string($label, $sizeLabel): $result\n";
        } catch (Throwable $e) {
            echo "FFI::string($label, $sizeLabel): ", $e::class, ": ", $e->getMessage(), "\n";
        }
    }
}
?>
--EXPECT--
FFI::string($array, 3): 'abc'
FFI::string($array, 0): ''
FFI::string($array, -1): ValueError: FFI::string(): Argument #2 ($size) must be greater than or equal to 0
FFI::string($array, PHP_INT_MIN): ValueError: FFI::string(): Argument #2 ($size) must be greater than or equal to 0
FFI::string($pointer, 3): 'abc'
FFI::string($pointer, 0): ''
FFI::string($pointer, -1): ValueError: FFI::string(): Argument #2 ($size) must be greater than or equal to 0
FFI::string($pointer, PHP_INT_MIN): ValueError: FFI::string(): Argument #2 ($size) must be greater than or equal to 0

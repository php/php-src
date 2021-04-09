--TEST--
FFI 031: bit-fields packing
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
function test_size($expected_size, $type) {
    try {
        $size = FFI::sizeof(FFI::new($type));
        if ($size !== $expected_size) {
            echo "FAIL: sizeof($type) != $expected_size ($size)\n";
        }
    } catch (Throwable $e) {
        echo $type . "=>" . get_class($e) . ": " . $e->getMessage()."\n";
    }
}

test_size( 4, "struct {int a:2; int b:2;}");
test_size( 1, "struct __attribute__((packed)) {int a:2; int b:2;}");
test_size( 8, "struct {int a:2; unsigned long long :60; int b:2;}");
test_size( 9, "struct __attribute__((packed)) {int a:2; unsigned long long :64; int b:2;}");
test_size( 4, "union {int a:2; int b:8;}");
test_size( 1, "union __attribute__((packed)) {int a:2; int b:8;}");
?>
ok
--EXPECT--
ok

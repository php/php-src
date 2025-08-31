--TEST--
FFI 021: packed enums
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
function test($size, $type) {
    if (FFI::sizeof(FFI::new($type)) !== $size) {
        echo "FAIL: sizeof($type) != $size\n";
    }
}

test(4, "enum {a1, b1}");
test(1, "enum __attribute__((packed)) {a2, b2}");

test(4, "enum {a3=0, b3=0x80000000}");
test(8, "enum {a4=-1, b4=0x80000000}");
test(8, "enum {a5=0x80000000, b5=-1}");
test(4, "enum {a6=-1, b6=0x7fffffff}");
test(8, "enum {a7=-1, b7=0x7fffffff, c7}");

test(1, "enum __attribute__((packed)) {a8=0, b8=0xff}");
test(2, "enum __attribute__((packed)) {a9=0, b9=0x100}");

test(1, "enum __attribute__((packed)) {a10=-1, b10=0x7f}");
test(2, "enum __attribute__((packed)) {a11=-1, b11=0x80}");
test(1, "enum __attribute__((packed)) {a12=0x7f, b12=-0x80}");
test(2, "enum __attribute__((packed)) {a13=0x7f, b13=-0x81}");

test(2, "enum __attribute__((packed)) {a8=0, b8=0xffff}");
test(4, "enum __attribute__((packed)) {a9=0, b9=0x10000}");

test(2, "enum __attribute__((packed)) {a10=-1, b10=0x7f00}");
test(4, "enum __attribute__((packed)) {a11=-1, b11=0x8000}");
test(2, "enum __attribute__((packed)) {a12=0x7f00, b12=-0x8000}");
test(4, "enum __attribute__((packed)) {a13=0x7f00, b13=-0x8001}");

test(4, "enum __attribute__((packed)) {a8=0, b8=0xffffffff}");
test(8, "enum __attribute__((packed)) {a9=0, b9=0x100000000}");

test(4, "enum __attribute__((packed)) {a10=-1, b10=0x7f000000}");
test(8, "enum __attribute__((packed)) {a11=-1, b11=0x80000000}");
test(4, "enum __attribute__((packed)) {a12=0x7f000000, b12=-0x80000000}");
test(8, "enum __attribute__((packed)) {a13=0x7f000000, b13=-0x80000001}");

test(1, "enum __attribute__((packed)) {a14=-0x80}");
test(2, "enum __attribute__((packed)) {a14=-0x81}");
test(2, "enum __attribute__((packed)) {a14=-0x8000}");
test(4, "enum __attribute__((packed)) {a14=-0x8001}");
test(4, "enum __attribute__((packed)) {a14=-0x80000000}");
test(8, "enum __attribute__((packed)) {a14=-0x80000001}");
?>
ok
--EXPECTF--
Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d

Deprecated: Calling FFI::new() statically is deprecated in %s on line %d
ok

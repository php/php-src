--TEST--
FFI 022: structure/union alignment
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
function test_size($size, $type) {
    if (FFI::sizeof(FFI::new($type)) !== $size) {
        echo "FAIL: sizeof($type) != $size\n";
    }
}

function test_align($align, $type) {
    if (FFI::alignof(FFI::new($type)) !== $align) {
        echo "FAIL: alignof($type) != $align\n";
    }
}

test_size(8, "struct {uint32_t a; uint32_t b;}");
test_size(3, "struct {char a; uint8_t b; uint8_t c;}");
test_size(8, "struct {char a; uint32_t b;}");
test_size(8, "struct {uint32_t a; char b;}");
test_size(5, "struct __attribute__((packed)) {char a; uint32_t b;}");
test_size(5, "struct __attribute__((packed)) {uint32_t a; char b;}");

test_size(16, "struct {uint32_t a; uint32_t b;}[2]");
test_size(6,  "struct {char a; uint8_t b; uint8_t c;}[2]");
test_size(16, "struct {char a; uint32_t b;}[2]");
test_size(16, "struct {uint32_t a; char b;}[2]");
test_size(10, "struct __attribute__((packed)) {char a; uint32_t b;}[2]");
test_size(10, "struct __attribute__((packed)) {uint32_t a; char b;}[2]");

test_align(4, "union {uint32_t a; uint32_t b;}");
test_align(1, "union {char a; uint8_t b; uint8_t c;}");
test_align(4, "union {char a; uint32_t b;}");
test_align(4, "union {uint32_t a; char b;}");
test_align(1, "union __attribute__((packed)) {char a; uint32_t b;}");
test_align(1, "union __attribute__((packed)) {uint32_t a; char b;}");

test_size(8, "struct {char a __attribute__((packed)); uint32_t b;}");
test_size(5, "struct {char a; uint32_t b  __attribute__((packed));}");
test_size(5, "struct {uint32_t a __attribute__((packed)); char b;}");
test_size(8, "struct {uint32_t a; char b __attribute__((packed));}");

test_align(4, "struct {char a __attribute__((packed)); uint32_t b;}");
test_align(1, "struct {char a; uint32_t b  __attribute__((packed));}");
test_align(1, "struct {uint32_t a __attribute__((packed)); char b;}");
test_align(4, "struct {uint32_t a; char b __attribute__((packed));}");

test_size(16, "struct  __attribute__((aligned(16))) {char a; uint32_t b;}");
test_align(16, "struct  __attribute__((aligned(16))) {char a; uint32_t b;}");

test_size(16, "struct {char a; uint32_t b;} __attribute__((aligned(16)))");
test_align(16, "struct {char a; uint32_t b;} __attribute__((aligned(16)))");

test_size(8, "struct  {char a; uint32_t b __attribute__((aligned(1)));}");
test_align(4, "struct  {char a; uint32_t b __attribute__((aligned(1)));}");
test_size(32, "struct {char a; uint32_t b __attribute__((aligned(16)));}");
test_align(16, "struct {char a; uint32_t b __attribute__((aligned(16)));}");

if (substr(PHP_OS, 0, 3) != 'WIN') {
    test_size(FFI::__BIGGEST_ALIGNMENT__ * 2, "struct  {char a; uint32_t b __attribute__((aligned));}");
    test_align(FFI::__BIGGEST_ALIGNMENT__, "struct  {char a; uint32_t b __attribute__((aligned));}");
}

test_size(16, "struct  __declspec(align(16)) {char a; uint32_t b;}");
test_align(16, "struct  __declspec(align(16)) {char a; uint32_t b;}");
?>
ok
--EXPECT--
ok

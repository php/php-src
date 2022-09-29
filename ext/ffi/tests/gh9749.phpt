--TEST--
Disallow instantiation of unsized types
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php

function test(string $type) {
    echo $type . ': ';
    try {
        FFI::new($type);
        echo 'sized';
    } catch (Throwable $e) {
        echo $e->getMessage();
    }
    echo "\n";
}

test('char[0]');
test('char[1]');
test('char*');
test('struct {}');
test('struct {}*');
test('struct {int32_t length; char data[];}');
test('struct {int32_t length; char data[0];}');
test('union {}');
test('union {}*');
test('enum { FOO }');

?>
--EXPECT--
char[0]: Cannot instantiate FFI\CData of zero size
char[1]: sized
char*: sized
struct {}: Cannot instantiate FFI\CData of zero size
struct {}*: sized
struct {int32_t length; char data[];}: Cannot instantiate FFI\CData of unsized type
struct {int32_t length; char data[0];}: Cannot instantiate FFI\CData of unsized type
union {}: Cannot instantiate FFI\CData of zero size
union {}*: sized
enum { FOO }: sized

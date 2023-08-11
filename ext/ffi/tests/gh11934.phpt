--TEST--
Feature GH-11934 (Allow to pass CData into struct and/or union fields)
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php

$cdef = FFI::cdef();

echo "--- Primitive types ---\n";

// Choose integer numbers off the maximum to test copy
$types = [
    'uint8_t' => 200,
    'uint16_t' => 16000,
    'uint32_t' => 1000_000,
    'uint64_t' => PHP_INT_MAX - 100,
    'int8_t' => -100,
    'int16_t' => -16000,
    'int32_t' => -1000_000,
    'int64_t' => PHP_INT_MIN + 100,
    'char' => 'F',
    'bool' => false,
    'float' => 1.00,
    'double' => -1.00,
];

// Positive test
foreach ($types as $type => $value) {
    $source = $cdef->new($type);
    $source->cdata = $value;
    $struct = $cdef->new("struct { $type field; }");
    $struct->field = $source;
    echo "Positive test $type: ";
    var_dump($struct->field === $value);
}

// Negative test
$dummy = $cdef->new("int[2]");
foreach ($types as $type => $value) {
    $struct = $cdef->new("struct { int field; }");
    $struct->field = $dummy;
}

// Enum test
$enum_definition = "enum { A, B, C, D }";
$source = $cdef->new($enum_definition);
$source->cdata = 2;
$struct = $cdef->new("struct { $enum_definition field; }");
$struct->field = $source;
echo "Positive test enum: ";
var_dump($struct->field === $source->cdata);
$struct->field = $dummy;

echo "--- Struct ---\n";

// Nested structs
$cdef = FFI::cdef("
    struct inner_struct {
        int data[1];
    };
    struct my_struct {
        int foo;
        int bar;
        struct inner_struct inner;
    };
    struct my_nesting_struct {
        struct my_struct field;
    };");
$source = $cdef->new("struct my_struct");
$source->foo = 123;
$source->bar = 456;
$source->inner->data[0] = 789;
$struct = $cdef->new("struct my_nesting_struct");
$struct->field = $source;
var_dump($struct->field->foo === 123 && $struct->field->bar === 456 && $struct->field->inner->data[0] === 789);

echo "--- Callback return type ---\n";

$ffi = FFI::cdef('
typedef uint32_t (*test_callback)();
typedef struct {
	test_callback call_me;
} my_struct;
');

$struct = $ffi->new('my_struct');
$struct->call_me = function () use ($ffi) {
	$int = $ffi->new('uint32_t');
	$int->cdata = 42;
	return $int;
};

var_dump(($struct->call_me)());

echo "--- Other FFI\CData assignment ---\n";

$ffi = FFI::cdef('');

$source = $ffi->new('uint32_t');
$source->cdata = 123;
$target = $ffi->new('uint32_t');
$target->cdata = $source;

var_dump($target->cdata);

echo "--- Array element ---\n";

$ffi = FFI::cdef('');

$source = $ffi->new('uint32_t');
$source->cdata = 123;
$target = $ffi->new('uint32_t[1]');
$target[0] = $source;

var_dump($target[0]);

?>
--EXPECTF--
--- Primitive types ---
Positive test uint8_t: bool(true)
Positive test uint16_t: bool(true)
Positive test uint32_t: bool(true)
Positive test uint64_t: bool(true)
Positive test int8_t: bool(true)
Positive test int16_t: bool(true)
Positive test int32_t: bool(true)
Positive test int64_t: bool(true)
Positive test char: bool(true)
Positive test bool: bool(true)
Positive test float: bool(true)
Positive test double: bool(true)

Warning: Object of class FFI\CData could not be converted to int in %s on line %d

Warning: Object of class FFI\CData could not be converted to int in %s on line %d

Warning: Object of class FFI\CData could not be converted to int in %s on line %d

Warning: Object of class FFI\CData could not be converted to int in %s on line %d

Warning: Object of class FFI\CData could not be converted to int in %s on line %d

Warning: Object of class FFI\CData could not be converted to int in %s on line %d

Warning: Object of class FFI\CData could not be converted to int in %s on line %d

Warning: Object of class FFI\CData could not be converted to int in %s on line %d

Warning: Object of class FFI\CData could not be converted to int in %s on line %d

Warning: Object of class FFI\CData could not be converted to int in %s on line %d

Warning: Object of class FFI\CData could not be converted to int in %s on line %d

Warning: Object of class FFI\CData could not be converted to int in %s on line %d
Positive test enum: bool(true)

Warning: Object of class FFI\CData could not be converted to int in %s on line %d
--- Struct ---
bool(true)
--- Callback return type ---
int(42)
--- Other FFI\CData assignment ---
int(123)
--- Array element ---
int(123)

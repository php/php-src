--TEST--
FFI 045: FFI\CType::getName()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef("struct timezone {
    int tz_minuteswest;
    int tz_dsttime;
};");
$type0 = FFI::typeof($ffi->new('struct timezone'));
var_dump($type0->getName());
$type1 = FFI::typeof(FFI::new('char'));
var_dump($type1->getName());
$type2 = FFI::typeof(FFI::new("int32_t*"));
var_dump($type2->getName());
var_dump(FFI::type('char')->getName());
var_dump(FFI::type('int32_t*')->getName());
var_dump($ffi->type('struct timezone')->getName());
?>
--EXPECT--
string(15) "struct timezone"
string(4) "char"
string(8) "int32_t*"
string(4) "char"
string(8) "int32_t*"
string(15) "struct timezone"

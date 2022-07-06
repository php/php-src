--TEST--
FFI 044: mode attribute
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef("
typedef int a __attribute__ ((__mode__ (__QI__)));
typedef unsigned int ua __attribute__ ((__mode__ (__QI__)));
typedef int b __attribute__ ((__mode__ (__HI__)));
typedef unsigned int ub __attribute__ ((__mode__ (__HI__)));
typedef int c __attribute__ ((__mode__ (__SI__)));
typedef unsigned int uc __attribute__ ((__mode__ (__SI__)));
typedef int d __attribute__ ((__mode__ (__DI__)));
typedef unsigned int ud __attribute__ ((__mode__ (__DI__)));
typedef float e __attribute__ ((__mode__ (__SF__)));
typedef float f __attribute__ ((__mode__ (__DF__)));
");
var_dump(FFI::sizeof($ffi->new("a")));
var_dump(FFI::sizeof($ffi->new("ua")));
var_dump(FFI::sizeof($ffi->new("b")));
var_dump(FFI::sizeof($ffi->new("ub")));
var_dump(FFI::sizeof($ffi->new("c")));
var_dump(FFI::sizeof($ffi->new("uc")));
var_dump(FFI::sizeof($ffi->new("d")));
var_dump(FFI::sizeof($ffi->new("ud")));
var_dump(FFI::sizeof($ffi->new("e")));
var_dump(FFI::sizeof($ffi->new("f")));
?>
--EXPECT--
int(1)
int(1)
int(2)
int(2)
int(4)
int(4)
int(8)
int(8)
int(4)
int(8)

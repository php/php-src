--TEST--
FFI 044: mode attribute
--EXTENSIONS--
ffi
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
var_dump(FFI::sizeof(FFI::new("a", cdef : $ffi)));
var_dump(FFI::sizeof(FFI::new("ua", cdef : $ffi)));
var_dump(FFI::sizeof(FFI::new("b", cdef : $ffi)));
var_dump(FFI::sizeof(FFI::new("ub", cdef : $ffi)));
var_dump(FFI::sizeof(FFI::new("c", cdef : $ffi)));
var_dump(FFI::sizeof(FFI::new("uc", cdef : $ffi)));
var_dump(FFI::sizeof(FFI::new("d", cdef : $ffi)));
var_dump(FFI::sizeof(FFI::new("ud", cdef : $ffi)));
var_dump(FFI::sizeof(FFI::new("e", cdef : $ffi)));
var_dump(FFI::sizeof(FFI::new("f", cdef : $ffi)));
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

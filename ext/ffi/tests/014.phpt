--TEST--
FFI 014: Size of nested types
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef();

var_dump(FFI::sizeof($ffi->new("uint32_t[2]")));
var_dump(FFI::sizeof($ffi->new("uint32_t([2])")));
var_dump(FFI::sizeof($ffi->new("uint32_t([2])[2]")));
?>
ok
--EXPECT--
int(8)
int(8)
int(16)
ok
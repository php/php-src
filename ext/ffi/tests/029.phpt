--TEST--
FFI 029: _Alignas
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef("
    typedef char t1;
    typedef char _Alignas(int) t2;
");
var_dump(FFI::sizeof(FFI::new("struct {char a; t1 b;}", cdef : $ffi)));
var_dump(FFI::sizeof(FFI::new("struct {char a; t2 b;}", cdef : $ffi)));
?>
--EXPECT--
int(2)
int(8)

--TEST--
FFI 035: FFI::cdef()->new() not-owned
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$p = FFI::cdef()->new("uint16_t[2]", false);
var_dump($p);

FFI::free($p);
try {
    var_dump($p);
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
?>
--EXPECTF--
object(FFI\CData:uint16_t[2])#%d (2) {
  [0]=>
  int(0)
  [1]=>
  int(0)
}
object(FFI\CData:uint16_t[2])#%d (0) {
}
FFI\Exception: Use after free()

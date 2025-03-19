--TEST--
FFI/JIT 063: FFI::free()
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
;opcache.jit=tracing
opcache.jit_hot_loop=1
opcache.jit_hot_func=0
opcache.jit_hot_return=0
opcache.jit_hot_side_exit=0
;opcache.jit_debug=0x180005
--FILE--
<?php 
function test() {
  $ffi = FFI::cdef();
  for ($i = 0; $i < 5; $i++) {
  	$ret = $ffi->new("int", false);
  	FFI::free($ret);
  }
}
test();
?>
DONE
--EXPECT--
DONE

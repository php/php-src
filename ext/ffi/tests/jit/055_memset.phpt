--TEST--
FFI/JIT 055: FFI::memset()
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
  $x = $ffi->new("char[10]");
  for ($i = 0; $i < 5; $i++) {
  	FFI::memset($x, ord("a"), 9);
  }
  var_dump(FFI::string($x));
}
test();
?>
--EXPECT--
string(9) "aaaaaaaaa"

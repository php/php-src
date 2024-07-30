--TEST--
FFI/JIT 057: FFI::memcpy() (CData)
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
  $y = $ffi->new("char[10]");
  FFI::memcpy($y, "123456789", 9);
  for ($i = 0; $i < 5; $i++) {
  	FFI::memcpy($x, $y, 8);
  }
  var_dump(FFI::string($x), FFI::string($y));
}
test();
?>
--EXPECT--
string(8) "12345678"
string(9) "123456789"

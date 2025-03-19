--TEST--
FFI/JIT 059: FFI::memcmp() (string + CData)
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
  $x = $ffi->new("char[10]");
  FFI::memcpy($x, "123456789", 9);
  for ($i = 0; $i < 5; $i++) {
  	$ret = FFI::memcmp("124", $x, 3);
  }
  var_dump($ret);
}
test();
?>
--EXPECT--
int(1)

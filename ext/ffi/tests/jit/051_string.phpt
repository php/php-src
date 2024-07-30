--TEST--
FFI/JIT 051: FFI::string()
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
  $x[0] = "X";
  for ($i = 0; $i < 5; $i++) {
  	$ret = FFI::string($x, 1);
  }
  var_dump($ret);
}
test();
?>
--EXPECT--
string(1) "X"


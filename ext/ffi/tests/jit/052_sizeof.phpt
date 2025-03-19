--TEST--
FFI/JIT 052: FFI::sizeof()
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
  $x = $ffi->new("int[10]");
  $x[0] = 42;
  for ($i = 0; $i < 5; $i++) {
  	$ret = FFI::sizeof($x);
  }
  var_dump($ret);
}
test();
?>
--EXPECT--
int(40)

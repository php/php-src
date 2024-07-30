--TEST--
FFI/JIT 036: Scalar CData Read
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
  $a = FFI::cdef()->new("int");
  $a->cdata = 42;
  $x = 0;
  for ($i = 0; $i < 5; $i++) {
  	$x += $a->cdata;
  }
  var_dump($x);
}
test();
?>
--EXPECT--
int(210)

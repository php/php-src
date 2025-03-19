--TEST--
FFI/JIT 014: Read Array (nested array)
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
  $a = FFI::cdef()->new("int32_t[5][2]");
  $a[0][1] = 0;
  $a[1][1] = 2;
  $a[2][1] = 4;
  $a[3][1] = 6;
  $a[4][1] = 8;
  $x = 0;
  for ($i = 0; $i < 5; $i++) {
  	$x += $a[$i][1];
  }
  var_dump($x);
}
test();
?>
--EXPECTF--
int(20)

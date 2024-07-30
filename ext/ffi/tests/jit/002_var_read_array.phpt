--TEST--
FFI/JIT 002: Read Variable (array)
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
  $ffi = FFI::cdef(<<<EOF
    extern int8_t stdout[8];
    EOF, 'libc.so.6');

  $x = 0;
  for ($i = 0; $i < 5; $i++) {
  	$x = $ffi->stdout;
  }
  var_dump($x);
}
test();
?>
--EXPECTF--
object(FFI\CData:int8_t[8])#%d (8) {
  [0]=>
  int(%i)
  [1]=>
  int(%i)
  [2]=>
  int(%i)
  [3]=>
  int(%i)
  [4]=>
  int(%i)
  [5]=>
  int(%i)
  [6]=>
  int(%i)
  [7]=>
  int(%i)
}

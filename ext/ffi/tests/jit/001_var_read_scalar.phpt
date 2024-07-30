--TEST--
FFI/JIT 001: Read Variable (scalar)
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
    extern int64_t stdout;
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
int(%i)

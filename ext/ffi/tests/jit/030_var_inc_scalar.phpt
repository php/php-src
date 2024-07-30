--TEST--
FFI/JIT 030: Increment Variable (scalar)
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

  $ffi->stdout = null;
  for ($i = 0; $i < 5; $i++) {
  	$ffi->stdout++;
  }
  var_dump($ffi->stdout);
}
test();
?>
--EXPECT--
int(5)

--TEST--
FFI/JIT 007: Write Variable (scalar cdata)
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

  $x = $ffi->cast('int64_t', 42);
  var_dump($x);
  for ($i = 0; $i < 5; $i++) {
  	$ffi->stdout = $x;
  }
  var_dump($ffi->stdout);
}
test();
?>
--EXPECTF--
object(FFI\CData:int64_t)#%d (1) {
  ["cdata"]=>
  int(42)
}
int(42)

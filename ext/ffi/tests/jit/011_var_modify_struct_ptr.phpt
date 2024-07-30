--TEST--
FFI/JIT 011: Modify Variable (struct ptr)
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
    typedef struct _IO_FILE {int32_t x;} FILE;
    extern FILE *stdout;
    EOF, 'libc.so.6');

  $ffi->stdout = $ffi->cast('FILE*', 42);
  for ($i = 0; $i < 5; $i++) {
  	$ffi->stdout += $i;
  }
  var_dump($ffi->cast('intptr_t', $ffi->stdout));
}
test();
?>
--EXPECTF--
object(FFI\CData:int%d_t)#%d (1) {
  ["cdata"]=>
  int(82)
}

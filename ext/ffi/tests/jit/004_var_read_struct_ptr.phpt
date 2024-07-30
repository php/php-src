--TEST--
FFI/JIT 004: Read Variable (struct ptr)
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
    typedef struct _IO_FILE FILE;
    extern FILE *stdout;
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
object(FFI\CData:struct _IO_FILE*)#%d (1) {
  [0]=>
  object(FFI\CData:struct _IO_FILE)#%d (0) {
  }
}

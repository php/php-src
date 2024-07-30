--TEST--
FFI/JIT 008: Write Variable (struct ptr null)
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

  for ($i = 0; $i < 5; $i++) {
  	$ffi->stdout = null;
  }
  var_dump($ffi->stdout);
}
test();
?>
--EXPECT--
NULL

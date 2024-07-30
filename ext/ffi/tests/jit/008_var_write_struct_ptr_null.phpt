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
--SKIPIF--
<?php
try {
    $libc = FFI::cdef("extern uintptr_t stdout;", "libc.so.6");
} catch (Throwable $_) {
    die('skip libc.so.6 not available');
}
?>
--FILE--
<?php 
function test() {
  $ffi = FFI::cdef(<<<EOF
    typedef struct _IO_FILE FILE;
    extern FILE *stdout;
    EOF, 'libc.so.6');

  $old = $ffi->stdout;
  for ($i = 0; $i < 5; $i++) {
  	$ffi->stdout = null;
  }
  $out = $ffi->stdout;
  $ffi->stdout = $old;
  var_dump($out);
}
test();
?>
--EXPECT--
NULL

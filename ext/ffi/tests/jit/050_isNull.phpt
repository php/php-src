--TEST--
FFI/JIT 050: FFI::isNull()
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
--SKIPIF--
<?php
try {
    $libc = FFI::cdef("extern intptr_t stdout;", "libc.so.6");
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
  for ($i = 0; $i < 5; $i++) {
  	$ret = FFI::isNull($ffi->stdout);
  }
  var_dump($ret);
}
test();
?>
--EXPECT--
bool(false)

--TEST--
FFI/JIT 042: Function call (tmp CData arg)
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
    $libc = FFI::cdef("extern uintptr_t stdout; void fprintf(void);", "libc.so.6");
} catch (Throwable $_) {
    die('skip libc.so.6 not available');
}
?>
--FILE--
<?php 
function test($name) {
  $ffi = FFI::cdef(<<<EOF
    typedef struct _IO_FILE FILE;
    extern FILE *stdout;
    int fprintf(FILE *, const char *, ...);
    EOF, 'libc.so.6');
  for ($i = 0; $i < 5; $i++) {
  	$ffi->fprintf($ffi->stdout, "Hello %s\n", $name);
  }
}
test("FFI");
?>
--EXPECT--
Hello FFI
Hello FFI
Hello FFI
Hello FFI
Hello FFI

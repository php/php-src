--TEST--
FFI/JIT 040: Function call
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
function test($name) {
  $ffi = FFI::cdef(<<<EOF
    int printf(char *, ...);
    EOF, 'libc.so.6');
  for ($i = 0; $i < 5; $i++) {
  	$ffi->printf("Hello %s\n", $name);
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

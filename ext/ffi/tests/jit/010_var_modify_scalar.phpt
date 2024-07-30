--TEST--
FFI/JIT 010: Modify Variable (scalar)
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
    $libc = FFI::cdef("extern intptr_t stdout;");
} catch (Throwable $_) {
    die('skip libc.so.6 not available');
}
?>
--FILE--
<?php 
function test() {
  $ffi = FFI::cdef(<<<EOF
    extern int64_t stdout;
    EOF, 'libc.so.6');

  $old = $ffi->stdout;
  $ffi->stdout = null;
  for ($i = 0; $i < 5; $i++) {
  	$ffi->stdout += $i;
  }
  $out = $ffi->stdout;
  $ffi->stdout = $old;
  var_dump($out);
}
test();
?>
--EXPECT--
int(10)

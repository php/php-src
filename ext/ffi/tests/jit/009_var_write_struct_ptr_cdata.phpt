--TEST--
FFI/JIT 009: Write Variable (struct ptr cdata)
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
    typedef struct _IO_FILE FILE;
    extern FILE *stdout;
    EOF, 'libc.so.6');

  $old = $ffi->stdout;
  $x = $ffi->cast('FILE*', 42);
  var_dump($x);
  for ($i = 0; $i < 5; $i++) {
  	$ffi->stdout = $x;
  }
  var_dump($ffi->stdout);
  var_dump($ffi->cast('intptr_t', $ffi->stdout));
  $ffi->stdout = $old;
}
test();
?>
--EXPECTF--
object(FFI\CData:struct _IO_FILE*)#%d (1) {
  [0]=>
  object(FFI\CData:struct _IO_FILE)#%d (0) {
  }
}
object(FFI\CData:struct _IO_FILE*)#%d (1) {
  [0]=>
  object(FFI\CData:struct _IO_FILE)#%d (0) {
  }
}
object(FFI\CData:int%d_t)#%d (1) {
  ["cdata"]=>
  int(42)
}

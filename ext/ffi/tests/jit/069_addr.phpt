--TEST--
FFI/JIT 069: Function call with FFI::addr()
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
    typedef struct {char buf[10];} BUF;
    int sprintf(BUF *, char *, ...);
    EOF, 'libc.so.6');
  $x = $ffi->new("BUF");
  for ($i = 0; $i < 5; $i++) {
  	$ret = $ffi->sprintf(FFI::addr($x), "Hello %s", $name);
  }
  var_dump($ret, FFI::string($x->buf));
}
test("FFI");
?>
--EXPECT--
int(9)
string(9) "Hello FFI"

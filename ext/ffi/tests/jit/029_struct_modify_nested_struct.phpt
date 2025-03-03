--TEST--
FFI/JIT 029: Modify Struct (nested array)
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
--FILE--
<?php 
function test() {
  $a = FFI::cdef()->new("struct { struct { struct {int32_t x; int32_t y} a; } a; int32_t b;}");
  for ($i = 0; $i < 5; $i++) {
  	$a->a->a->x += $i;     
  }
  var_dump($a);
}
test();
?>
--EXPECTF--
object(FFI\CData:struct <anonymous>)#2 (2) {
  ["a"]=>
  object(FFI\CData:struct <anonymous>)#3 (1) {
    ["a"]=>
    object(FFI\CData:struct <anonymous>)#1 (2) {
      ["x"]=>
      int(10)
      ["y"]=>
      int(0)
    }
  }
  ["b"]=>
  int(0)
}

--TEST--
FFI/JIT 044: Write Struct (nested array - FETCH_DIM_W/VAR)
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
  $a = FFI::cdef()->new("struct {int32_t a[5][5]; int32_t b;}");
  for ($i = 0; $i < 5; $i++) {
  	$a->a[$i][$i] = $i;
  }
  var_dump($a);
}
test();
?>
--EXPECTF--
object(FFI\CData:struct <anonymous>)#%d (2) {
  ["a"]=>
  object(FFI\CData:int32_t[5][5])#%d (5) {
    [0]=>
    object(FFI\CData:int32_t[5])#%d (5) {
      [0]=>
      int(0)
      [1]=>
      int(0)
      [2]=>
      int(0)
      [3]=>
      int(0)
      [4]=>
      int(0)
    }
    [1]=>
    object(FFI\CData:int32_t[5])#%d (5) {
      [0]=>
      int(0)
      [1]=>
      int(1)
      [2]=>
      int(0)
      [3]=>
      int(0)
      [4]=>
      int(0)
    }
    [2]=>
    object(FFI\CData:int32_t[5])#%d (5) {
      [0]=>
      int(0)
      [1]=>
      int(0)
      [2]=>
      int(2)
      [3]=>
      int(0)
      [4]=>
      int(0)
    }
    [3]=>
    object(FFI\CData:int32_t[5])#%d (5) {
      [0]=>
      int(0)
      [1]=>
      int(0)
      [2]=>
      int(0)
      [3]=>
      int(3)
      [4]=>
      int(0)
    }
    [4]=>
    object(FFI\CData:int32_t[5])#%d (5) {
      [0]=>
      int(0)
      [1]=>
      int(0)
      [2]=>
      int(0)
      [3]=>
      int(0)
      [4]=>
      int(4)
    }
  }
  ["b"]=>
  int(0)
}

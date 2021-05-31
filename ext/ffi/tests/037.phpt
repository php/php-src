--TEST--
FFI 037: Type memory management
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
function foo($ptr) {
    $buf = FFI::new("int*[1]");
    $buf[0] = $ptr;
    //...
    return $buf[0];
}

$int = FFI::new("int");
$int->cdata = 42;
var_dump(foo(FFI::addr($int)));
?>
--EXPECTF--
object(FFI\CData:int32_t*)#%d (1) {
  [0]=>
  int(42)
}

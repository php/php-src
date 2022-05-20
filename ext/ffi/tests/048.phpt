--TEST--
FFI 046: FFI::getSymbols()
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php


$ffi = FFI::cdef(<<<EOF
    /* allowed storage classes */
    typedef  int type1;
EOF
);

var_dump(FFI::getSymbols($ffi));
var_dump(FFI::getSymbols($ffi, FFI::SYM_TYPE));
var_dump(FFI::getSymbols($ffi, FFI::SYM_FUNC));
?>
ok
--EXPECT--
array(1) {
  ["type1"]=>
  object(FFI\CType:int32_t)#2 (0) {
  }
}
array(1) {
  ["type1"]=>
  object(FFI\CType:int32_t)#3 (0) {
  }
}
array(0) {
}
ok
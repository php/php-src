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
    typedef  char type2;
    enum _g {
    _c1,
    _c2,
    _c3 = 1,
    _c4,
    };

EOF
);

var_dump(FFI::getSymbols($ffi));
var_dump(FFI::getSymbols($ffi, FFI::SYM_TYPE));
var_dump(FFI::getSymbols($ffi, FFI::SYM_FUNC));
var_dump(FFI::getSymbols($ffi, FFI::SYM_CONST));
?>
ok
--EXPECTF--
array(6) {
  ["type1"]=>
  object(FFI\CType:int32_t)#%d (0) {
  }
  ["type2"]=>
  object(FFI\CType:char)#%d (0) {
  }
  ["_c1"]=>
  object(FFI\CType:uint32_t)#%d (0) {
  }
  ["_c2"]=>
  object(FFI\CType:uint32_t)#%d (0) {
  }
  ["_c3"]=>
  object(FFI\CType:uint32_t)#%d (0) {
  }
  ["_c4"]=>
  object(FFI\CType:uint32_t)#%d (0) {
  }
}
array(2) {
  ["type1"]=>
  object(FFI\CType:int32_t)#%d (0) {
  }
  ["type2"]=>
  object(FFI\CType:char)#%d (0) {
  }
}
array(0) {
}
array(4) {
  ["_c1"]=>
  object(FFI\CType:uint32_t)#%d (0) {
  }
  ["_c2"]=>
  object(FFI\CType:uint32_t)#%d (0) {
  }
  ["_c3"]=>
  object(FFI\CType:uint32_t)#%d (0) {
  }
  ["_c4"]=>
  object(FFI\CType:uint32_t)#%d (0) {
  }
}
ok

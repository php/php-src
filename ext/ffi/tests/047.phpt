--TEST--
FFI 047: FFI::hasSymbol()
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

var_dump(FFI::hasSymbol($ffi, 'type1'));
var_dump(FFI::hasSymbol($ffi, 'type1', FFI::SYM_TYPE));
var_dump(FFI::hasSymbol($ffi, 'type1', FFI::SYM_FUNC));
?>
ok
--EXPECT--
bool(true)
bool(true)
bool(false)
ok
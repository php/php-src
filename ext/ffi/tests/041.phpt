--TEST--
FFI 041: Type memory management
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
function test_new_ownership($str, $transfer) {
    if ($transfer) {
        return FFI::new(FFI::type($str));
    } else {
        $type = FFI::type($str);
        return FFI::new($type);
    }
}
var_dump(test_new_ownership("int", 1));
var_dump(test_new_ownership("int[2]", 1));
var_dump(test_new_ownership("int", 0));
var_dump(test_new_ownership("int[2]", 0));

function test_type_ownership($str, $transfer) {
    if ($transfer) {
        return FFI::typeof(FFI::new($str));
    } else {
        $data = FFI::new($str);
        return FFI::typeof($data);
    }
}
var_dump(test_type_ownership("int", 1));
var_dump(test_type_ownership("int[2]", 1));
var_dump(test_type_ownership("int", 0));
var_dump(test_type_ownership("int[2]", 0));

function test_cast_ownership($str, $transfer) {
    if ($transfer) {
        return FFI::cast(FFI::type($str), FFI::new($str));
    } else {
        $type = FFI::type($str);
        return FFI::cast($type, FFI::new($str));
    }
}
var_dump(test_cast_ownership("int", 1));
var_dump(test_cast_ownership("int[2]", 1));
var_dump(test_cast_ownership("int", 0));
var_dump(test_cast_ownership("int[2]", 0));

function test_array_ownership($str, $transfer) {
    if ($transfer) {
        return FFI::arrayType(FFI::type($str), [2]);
    } else {
        $type = FFI::type($str);
        return FFI::arrayType($type, [2]);
    }
}
var_dump(test_array_ownership("int", 1));
var_dump(test_array_ownership("int[2]", 1));
var_dump(test_array_ownership("int", 0));
var_dump(test_array_ownership("int[2]", 0));
?>
--EXPECTF--
object(FFI\CData:int32_t)#%d (1) {
  ["cdata"]=>
  int(0)
}
object(FFI\CData:int32_t[2])#%d (2) {
  [0]=>
  int(0)
  [1]=>
  int(0)
}
object(FFI\CData:int32_t)#%d (1) {
  ["cdata"]=>
  int(0)
}
object(FFI\CData:int32_t[2])#%d (2) {
  [0]=>
  int(0)
  [1]=>
  int(0)
}
object(FFI\CType:int32_t)#%d (0) {
}
object(FFI\CType:int32_t[2])#%d (0) {
}
object(FFI\CType:int32_t)#%d (0) {
}
object(FFI\CType:int32_t[2])#%d (0) {
}
object(FFI\CData:int32_t)#%d (1) {
  ["cdata"]=>
  int(0)
}
object(FFI\CData:int32_t[2])#%d (2) {
  [0]=>
  int(0)
  [1]=>
  int(0)
}
object(FFI\CData:int32_t)#%d (1) {
  ["cdata"]=>
  int(0)
}
object(FFI\CData:int32_t[2])#%d (2) {
  [0]=>
  int(0)
  [1]=>
  int(0)
}
object(FFI\CType:int32_t[2])#%s (0) {
}
object(FFI\CType:int32_t[2][2])#%d (0) {
}
object(FFI\CType:int32_t[2])#%d (0) {
}
object(FFI\CType:int32_t[2][2])#%d (0) {
}

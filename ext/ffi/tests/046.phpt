--TEST--
FFI 046: FFI::CType reflection API
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$x = FFI::type("uint8_t");
var_dump($x->getKind() === $x::TYPE_UINT8);
var_dump($x->getSize());
var_dump($x->getAlignment());

$x = FFI::type("enum __attribute__((packed)) {a2, b2}");
var_dump($x->getKind() === $x::TYPE_ENUM);
var_dump($x->getEnumKind() === $x::TYPE_UINT8);

$x = FFI::type("char[5]");
var_dump($x->getKind() === $x::TYPE_ARRAY);
var_dump($x->getSize());
var_dump($x->getArrayElementType()->getKind() === $x::TYPE_CHAR);
var_dump($x->getArrayLength());

$x = FFI::type("void*");
var_dump($x->getKind() === $x::TYPE_POINTER);
var_dump($x->getPointerType()->getKind() === $x::TYPE_VOID);

$x = FFI::type("struct {double x; double y;}");
var_dump($x->getKind() === $x::TYPE_STRUCT);
var_dump(($x->getAttributes() & $x::ATTR_UNION) != 0);
var_dump($x->getStructFieldNames());
var_dump($x->getStructFieldOffset("x"));
var_dump($x->getStructFieldOffset("y"));
var_dump($x->getStructFieldType("x")->getKind() == $x::TYPE_DOUBLE);
var_dump($x->getStructFieldType("y")->getKind() == $x::TYPE_DOUBLE);

$x = FFI::type("union {double x; double y;}");
var_dump($x->getKind() === $x::TYPE_STRUCT);
var_dump(($x->getAttributes() & $x::ATTR_UNION) != 0);
var_dump($x->getStructFieldNames());
var_dump($x->getStructFieldOffset("x"));
var_dump($x->getStructFieldOffset("y"));
var_dump($x->getStructFieldType("x")->getKind() == $x::TYPE_DOUBLE);
var_dump($x->getStructFieldType("y")->getKind() == $x::TYPE_DOUBLE);

$x = FFI::type("void (*)(double,int32_t)");
var_dump($x->getKind() === $x::TYPE_POINTER);
var_dump($x->getPointerType()->getKind() === $x::TYPE_FUNC);
var_dump($x->getPointerType()->getFuncReturnType()->getKind() === $x::TYPE_VOID);
var_dump($x->getPointerType()->getFuncParameterCount());
var_dump($x->getPointerType()->getFuncParameterType(0)->getKind() === $x::TYPE_DOUBLE);
var_dump($x->getPointerType()->getFuncParameterType(1)->getKind() === $x::TYPE_SINT32);
?>
--EXPECT--
bool(true)
int(1)
int(1)
bool(true)
bool(true)
bool(true)
int(5)
bool(true)
int(5)
bool(true)
bool(true)
bool(true)
bool(false)
array(2) {
  [0]=>
  string(1) "x"
  [1]=>
  string(1) "y"
}
int(0)
int(8)
bool(true)
bool(true)
bool(true)
bool(true)
array(2) {
  [0]=>
  string(1) "x"
  [1]=>
  string(1) "y"
}
int(0)
int(0)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(2)
bool(true)
bool(true)

--TEST--
FFI Referencing temporary owned data transfers ownership
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = \FFI::cdef(<<<'CPP'
typedef struct {
    int8_t bar;
} Foo;
CPP);
$structPtr = \FFI::addr($ffi->new('Foo'));
var_dump($structPtr);
?>
--EXPECT--
object(FFI\CData:struct <anonymous>*)#3 (1) {
  [0]=>
  object(FFI\CData:struct <anonymous>)#2 (1) {
    ["bar"]=>
    int(0)
  }
}

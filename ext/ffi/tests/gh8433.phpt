--TEST--
GH-8433 (Assigning function pointers to structs in FFI leaks memory)
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php

$ffi = FFI::cdef("typedef struct { int a; } bar;");
$x = FFI::new("bar(*)(void)", cdef : $ffi);
FFI::addr($x)[0] = function() use ($ffi) {
    $bar = FFI::new("bar", cdef : $ffi);
    $bar->a = 2;
    return $bar;
};
var_dump($x());

?>
--EXPECTF--
object(FFI\CData:struct <anonymous>)#%d (1) {
  ["a"]=>
  int(2)
}

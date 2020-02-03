--TEST--
FFI 025: direct work with primitive types
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
    $x = FFI::new("int");
    $x->cdata = 5;
    var_dump($x);
    $x->cdata += 2;
    var_dump($x);
    echo "$x\n\n";
    unset($x);

    $x = FFI::new("char");
    $x->cdata = 'a';
    var_dump($x);
    $x->cdata++;
    var_dump($x);
    echo "$x\n\n";
    unset($x);
?>
--EXPECTF--
object(FFI\CData:int32_t)#%d (1) {
  ["cdata"]=>
  int(5)
}
object(FFI\CData:int32_t)#%d (1) {
  ["cdata"]=>
  int(7)
}
7

object(FFI\CData:char)#%d (1) {
  ["cdata"]=>
  string(1) "a"
}
object(FFI\CData:char)#%d (1) {
  ["cdata"]=>
  string(1) "b"
}
b

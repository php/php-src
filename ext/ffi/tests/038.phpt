--TEST--
FFI 038: Casting array to pointer
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$a = FFI::new("int[10]");
for ($i = 0; $i < 10; $i++) {
    $a[$i] = $i;
}
$p = FFI::cast("int*", $a);
var_dump($p[0]);
var_dump($p[2]);
vaR_dump($p)
?>
--EXPECTF--
int(0)
int(2)
object(FFI\CData:int%d_t*)#%d (1) {
  [0]=>
  int(0)
}

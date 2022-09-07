--TEST--
FFI 047: FFI::CData->cdata meaning
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$x = FFI::new("int");
$x->cdata = 42;
var_dump($x);

$x = FFI::new("int*");
try {
	$x->cdata = 42;
    var_dump($x);
} catch (Throwable $e) {
	echo $e->getMessage() . "\n";
}

$x = FFI::new("struct {int cdata;}");
try {
	$x->cdata = 42;
    var_dump($x);
} catch (Throwable $e) {
	echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
object(FFI\CData:int32_t)#%d (1) {
  ["cdata"]=>
  int(42)
}
Attempt to assign field 'cdata' of non C struct/union
object(FFI\CData:struct <anonymous>)#%d (1) {
  ["cdata"]=>
  int(42)
}

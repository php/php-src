--TEST--
Bug #79571 (FFI: var_dumping unions may segfault)
--SKIPIF--
<?php
if (!extension_loaded('ffi')) die('skip ffi extension not available');
if (pack('S', 0xABCD) !== pack('v', 0xABCD)) {
    die('skip for little-endian architectures only');
}
?>
--FILE--
<?php
$ffi = FFI::cdef(<<<EOF
    typedef union {
        int num;
        char *str;
    } my_union;
EOF);

$union = $ffi->new('my_union');
$union->num = 42;
var_dump($union);
var_dump($union->num);
?>
--EXPECTF--
object(FFI\CData:union <anonymous>)#%d (2) {
  ["num"]=>
  int(42)
  ["str"]=>
  string(4) "0x2a"
}
int(42)

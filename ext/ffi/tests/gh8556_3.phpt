--TEST--
GH-8556 (FFI\CData use-after-free after FFI object destroyed) - cast variation
--EXTENSIONS--
ffi
--SKIPIF--
<?php
try {
    $libc = FFI::cdef("char *getenv(char *);", "libc.so.6");
} catch (Throwable $_) {
    die('skip libc.so.6 not available');
}
?>
--INI--
ffi.enable=1
--FILE--
<?php
$c_cdef = FFI::cdef("typedef int T[5];");
$a = $c_cdef->new("T");
$a_casted_ptr = $c_cdef->cast("T*", $a);
$a_clone = clone $a;
var_dump($a, $a_casted_ptr, $a_clone);
unset($c_cdef);
var_dump($a, $a_casted_ptr);
unset($a);
unset($a_casted_ptr);
var_dump($a_clone);
?>
--EXPECT--
object(FFI\CData:int32_t[5])#2 (5) {
  [0]=>
  int(0)
  [1]=>
  int(0)
  [2]=>
  int(0)
  [3]=>
  int(0)
  [4]=>
  int(0)
}
object(FFI\CData:int32_t(*)[5])#3 (1) {
  [0]=>
  NULL
}
object(FFI\CData:int32_t[5])#4 (5) {
  [0]=>
  int(0)
  [1]=>
  int(0)
  [2]=>
  int(0)
  [3]=>
  int(0)
  [4]=>
  int(0)
}
object(FFI\CData:int32_t[5])#2 (5) {
  [0]=>
  int(0)
  [1]=>
  int(0)
  [2]=>
  int(0)
  [3]=>
  int(0)
  [4]=>
  int(0)
}
object(FFI\CData:int32_t(*)[5])#3 (1) {
  [0]=>
  NULL
}
object(FFI\CData:int32_t[5])#4 (5) {
  [0]=>
  int(0)
  [1]=>
  int(0)
  [2]=>
  int(0)
  [3]=>
  int(0)
  [4]=>
  int(0)
}

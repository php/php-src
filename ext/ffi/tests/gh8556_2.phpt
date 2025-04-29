--TEST--
GH-8556 (FFI\CData use-after-free after FFI object destroyed) - new variation
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
$cdef = FFI::cdef("typedef int T[5];");
$a = $cdef->new("T");
var_dump($a);
unset($cdef);
var_dump($a);
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

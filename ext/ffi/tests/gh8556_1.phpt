--TEST--
GH-8556 (FFI\CData use-after-free after FFI object destroyed) - trampoline variation
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

putenv("TEST=123");

// Original
$c = (FFI::cdef("char *getenv(char *);", "libc.so.6"))->getenv("TEST");
var_dump($c);

// Variant on the original
$libc = FFI::cdef("char *getenv(char *);", "libc.so.6");
$result = $libc->getenv("TEST");
unset($libc); // Frees ffi cdata ->type member
var_dump($result);

?>
--EXPECT--
object(FFI\CData:char*)#2 (1) {
  [0]=>
  string(1) "1"
}
object(FFI\CData:char*)#4 (1) {
  [0]=>
  string(1) "1"
}

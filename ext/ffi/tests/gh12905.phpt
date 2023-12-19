--TEST--
GH-12905 (FFI::new interacts badly with observers)
--EXTENSIONS--
ffi
zend_test
--SKIPIF--
<?php
try {
    $libc = FFI::cdef("int printf(const char *format, ...);", "libc.so.6");
} catch (Throwable $_) {
    die('skip libc.so.6 not available');
}
?>
--INI--
ffi.enable=1
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=0
--FILE--
<?php
$ffi = FFI::cdef("", "libc.so.6");
$ffi->new("int");
?>
--EXPECTF--
<!-- init '%sgh12905.php' -->
<file '%sgh12905.php'>
  <!-- init FFI::cdef() -->
  <FFI::cdef>
  </FFI::cdef>
  <!-- init FFI::new() -->
  <FFI::new>
  </FFI::new>
</file '%sgh12905.php'>

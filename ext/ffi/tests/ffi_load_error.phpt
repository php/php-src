--TEST--
Test that FFI::load returns a detailed error when failling to load a shared library
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
try {
    FFI::load(__DIR__ . "/ffi_load_error.h");
} catch (FFI\Exception $ex) {
    printf($ex->getMessage());
}
?>
--EXPECTF--
Failed loading '%s' (%s)

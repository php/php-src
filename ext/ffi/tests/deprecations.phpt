--TEST--
Test calling the deprecated signature of FFI::new(), FFI::type(), FFI::cast()
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php

$p1 = FFI::new("uint8_t[2]");

set_error_handler(function ($severity, $message, $file, $line) {
    throw new Exception($message);
});
try {
    FFI::new("uint8_t[2]");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
set_error_handler(null);

var_dump($p1);

$t1 = FFI::type("uint16_t[2]");

set_error_handler(function ($severity, $message, $file, $line) {
    throw new Exception($message);
});
try {
    FFI::type("uint16_t[2]");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
set_error_handler(null);

$p = FFI::cast("uint8_t[2]", $p1);
set_error_handler(function ($severity, $message, $file, $line) {
    throw new Exception($message);
});
try {
    FFI::cast("uint8_t[2]", $p1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
Deprecated: Calling FFI::new() statically is deprecated in %s on line %d
Exception: Calling FFI::new() statically is deprecated
object(FFI\CData:uint8_t[2])#1 (2) {
  [0]=>
  int(0)
  [1]=>
  int(0)
}

Deprecated: Calling FFI::type() statically is deprecated in %s on line %d
Exception: Calling FFI::type() statically is deprecated

Deprecated: Calling FFI::cast() statically is deprecated in %s on line %d
Exception: Calling FFI::cast() statically is deprecated

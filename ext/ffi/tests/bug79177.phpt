--TEST--
Bug #79177 (FFI doesn't handle well PHP exceptions within callback)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('utils.inc');
try {
    ffi_cdef("extern void *zend_printf;", ffi_get_php_dll_name());
} catch (Throwable $e) {
    die('skip PHP symbols not available');
}
?>
--FILE--
<?php
require_once('utils.inc');
$php = ffi_cdef("
typedef char (*zend_write_func_t)(const char *str, size_t str_length);
extern zend_write_func_t zend_write;
", ffi_get_php_dll_name());

echo "Before\n";

$originalHandler = clone $php->zend_write;
$php->zend_write = function($str, $len): string {
    throw new \RuntimeException('Not allowed');
};
try { 
    echo "After\n";
} catch (\Throwable $exception) {
    // Do not output anything here, as handler is overridden
} finally {
    $php->zend_write = $originalHandler;
}
if (isset($exception)) {
    echo $exception->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
Before

Warning: Uncaught RuntimeException: Not allowed in %s:%d
Stack trace:
#0 %s(%d): {closure}('After\n', 6)
#1 {main}
  thrown in %s on line %d

Fatal error: Throwing from FFI callbacks is not allowed in %s on line %d

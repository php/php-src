--TEST--
Bug #79177 (FFI doesn't handle well PHP exceptions within callback)
--EXTENSIONS--
ffi
zend_test
--FILE--
<?php
require_once __DIR__ . '/utils.inc';
$header = <<<HEADER
extern int *(*bug79177_cb)(void);
void bug79177(void);
HEADER;

if (PHP_OS_FAMILY !== 'Windows') {
    $ffi = FFI::cdef($header);
} else {
    try {
        $ffi = FFI::cdef($header, 'php_zend_test.dll');
    } catch (FFI\Exception $ex) {
        $ffi = FFI::cdef($header, ffi_get_php_dll_name());
    }
}

$ffi->bug79177_cb = function() {
    throw new \RuntimeException('Not allowed');
};
try { 
    $ffi->bug79177(); // this is supposed to raise a fatal error
} catch (\Throwable $exception) {}
echo "done\n";
?>
--EXPECTF--
Warning: Uncaught RuntimeException: Not allowed in %s:%d
Stack trace:
#0 %s(%d): {closure}()
#1 %s(%d): FFI->bug79177()
#2 {main}
  thrown in %s on line %d

Fatal error: Throwing from FFI callbacks is not allowed in %s on line %d

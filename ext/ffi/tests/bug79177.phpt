--TEST--
Bug #79177 (FFI doesn't handle well PHP exceptions within callback)
--EXTENSIONS--
ffi
zend_test
--FILE--
<?php
$header = <<<HEADER
extern int *(*bug79177_cb)(void);
void bug79177(void);
HEADER;

$ffi = FFI::cdef($header);
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
#0 %s(%d): {closure:%s:%d}()
#1 %s(%d): FFI->bug79177()
#2 {main}
  thrown in %s on line %d

Fatal error: Throwing from FFI callbacks is not allowed in %s on line %d

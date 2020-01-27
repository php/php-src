--TEST--
Bug #79177 (FFI doesn't handle well PHP exceptions within callback body)
--SKIPIF--
<?php require_once('skipif.inc');?>
--INI--
ffi.enable=1
--FILE--
<?php

use FFI\CData;

$php = FFI::cdef("
typedef unsigned int fake_struct;
typedef fake_struct* (*zend_write_func_t)(const char *str, size_t str_length);
extern zend_write_func_t zend_write;
");

$originalHandler = clone $php->zend_write;
$php->zend_write = function($str, $len): CData {
    throw new \RuntimeException('Not allowed');
};
try {
    echo "After", PHP_EOL;
} catch (\Throwable $exception) {
    // Do not output anything here, as handler is overridden
} finally {
    $php->zend_write = $originalHandler;
}
echo get_class($exception), ': ', $exception->getMessage(), PHP_EOL;
?>
--EXPECT--
RuntimeException: Not allowed

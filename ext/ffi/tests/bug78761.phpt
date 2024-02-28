--TEST--
Bug #78761 (Zend memory heap corruption with preload and casting)
--EXTENSIONS--
ffi
--INI--
ffi.preload={PWD}/bug78761_preload.h
--FILE--
<?php
try {
    FFI::cdef()->cast('char[10]', FFI::cdef()->new('char[1]'));
} catch (FFI\Exception $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
attempt to cast to larger type

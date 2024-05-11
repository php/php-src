--TEST--
GH-9698 (stream_wrapper_register crashes with FFI\CData provided as class)
--EXTENSIONS--
ffi
--SKIPIF--
<?php
?>
--FILE--
<?php
try {
    stream_wrapper_register('badffi', 'FFI\CData');
    file_get_contents('badffi://x');
} catch (Throwable $exception) {
    echo $exception->getMessage();
}
?>

DONE
--EXPECT--
Attempt to assign field 'context' to uninitialized FFI\CData object
DONE

--TEST--
JIT TYPE_CHECK: 001 exception handling
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
set_error_handler(function($no, $msg) {
    throw new Exception($msg);
});

try {
    if (!is_scalar($a)) {
        undefined_function('Null');
    }
} catch (Exception $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Exception: Undefined variable $a

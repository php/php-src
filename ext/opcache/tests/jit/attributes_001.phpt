--TEST--
JIT disabled with attributes, assert no jit debug output
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.jit=1205
opcache.jit_debug=1
opcache.protect_memory=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
@@Jit("off")
function foo($a) {
    return $a * $a;
}
?>
okey
--EXPECT--
okey

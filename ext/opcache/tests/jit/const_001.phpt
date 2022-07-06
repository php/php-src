--TEST--
JIT CONST: defined
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function define_const() {
    define("CUSTOM_CONSTANT", 1);
}
function test_defined() {
    var_dump(defined("CUSTOM_CONSTANT"));
    define_const();
    var_dump(defined("CUSTOM_CONSTANT"));
}

test_defined();
?>
--EXPECT--
bool(false)
bool(true)

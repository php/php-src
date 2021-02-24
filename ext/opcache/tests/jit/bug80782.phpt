--TEST--
Bug #80782 (DASM_S_RANGE_VREG on PHP_INT_MIN-1)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
define('LONG_MIN', PHP_INT_MIN);
var_dump(LONG_MIN-1);
?>
--EXPECTF--
float(%s)

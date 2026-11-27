--TEST--
JIT: a negative opcache.jit_buffer_size is rejected
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=-1
--EXTENSIONS--
opcache
--SKIPIF--
<?php if (ini_get('opcache.jit_buffer_size') === false) die('skip JIT is not built'); ?>
--FILE--
<?php
var_dump(ini_get('opcache.jit_buffer_size'));
?>
--EXPECT--
Warning: Invalid "opcache.jit_buffer_size" setting. Should be greater than or equal to 0 in Unknown on line 0
string(3) "64M"

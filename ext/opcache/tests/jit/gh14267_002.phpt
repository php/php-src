--TEST--
GH-14267: JIT cannot be enabled at runtime
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.jit_buffer_size=32M
--EXTENSIONS--
opcache
--FILE--
<?php
ini_set('opcache.jit', 'tracing');
?>
--EXPECTF--
Warning: Cannot change opcache.jit setting at run-time (JIT is disabled) in %s on line %d

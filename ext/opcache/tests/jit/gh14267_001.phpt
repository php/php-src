--TEST--
GH-14267: JIT cannot be enabled at runtime
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=off
opcache.jit_buffer_size=32M
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (!(opcache_get_status()['jit']['enabled'] ?? false)) die('skip JIT is not available');
?>
--FILE--
<?php
ini_set('opcache.jit', 'tracing');
?>
===DONE===
--EXPECT--
===DONE===

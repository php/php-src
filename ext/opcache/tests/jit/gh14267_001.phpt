--TEST--
GH-14267: JIT cannot be enabled at runtime
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=off
opcache.jit_buffer_size=32M
--EXTENSIONS--
opcache
--FILE--
<?php
// Skip when JIT was completely disabled at runtime.
if (($status = opcache_get_status()) === false || $status['jit']['enabled']) {
    ini_set('opcache.jit', 'tracing');
}
?>
===DONE===
--EXPECT--
===DONE===

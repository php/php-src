--TEST--
JIT should not emit warning with opcache.jit_buffer_size=0
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=0
opcache.log_verbosity_level=2
--EXTENSIONS--
opcache
--FILE--
<?php
var_dump(opcache_get_status()['jit']['enabled'] ?? false);
?>
--EXPECT--
bool(false)

--TEST--
JIT: opcache.jit_max_root_traces=0 is rejected
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=16M
opcache.jit_max_root_traces=0
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if ((opcache_get_status()['jit']['on'] ?? false) === false) die('skip: JIT is not available');
?>
--FILE--
<?php
var_dump(ini_get("opcache.jit_max_root_traces"));
?>
--EXPECTF--
Warning: Invalid "opcache.jit_max_root_traces" setting. Should be between 1 and %d in Unknown on line 0
string(4) "1024"

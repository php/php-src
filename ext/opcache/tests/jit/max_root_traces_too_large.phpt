--TEST--
JIT: opcache.jit_max_root_traces beyond the uint32_t trace ids is rejected
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=16M
opcache.jit_max_root_traces=4294967296
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip 64-bit only");
if ((opcache_get_status()['jit']['on'] ?? false) === false) die('skip: JIT is not available');
?>
--FILE--
<?php
var_dump(ini_get("opcache.jit_max_root_traces"));
?>
--EXPECTF--
Warning: Invalid "opcache.jit_max_root_traces" setting. Should be between 1 and 4294967295 in Unknown on line 0
string(4) "1024"

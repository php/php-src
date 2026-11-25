--TEST--
JIT: opcache.jit_max_root_traces whose root traces buffer does not fit into shared memory
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=16M
opcache.memory_consumption=64
opcache.jit_max_root_traces=4294967295
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip 64-bit only");
/* The fatal error exits during startup, leaving everything allocated so far unfreed */
if (getenv('SKIP_ASAN')) die('xleak Leaks memory with ASAN');
if ((opcache_get_status()['jit']['on'] ?? false) === false) die('skip: JIT is not available');
?>
--FILE--
<?php
var_dump(ini_get("opcache.jit_max_root_traces"));
?>
--EXPECTF--
%sFatal Error Could not allocate JIT root traces buffer of %d bytes for opcache.jit_max_root_traces=4294967295, increase opcache.memory_consumption or decrease opcache.jit_max_root_traces

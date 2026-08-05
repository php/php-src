--TEST--
GH-18847 (SEGV in zend_fetch_debug_backtrace when the tracing JIT enters a frame and the memory limit is hit before opline is set)
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=8M
fatal_error_backtraces=1
memory_limit=8M
--FILE--
<?php
function f() {
    static $x = f();
}
f();
?>
--EXPECTREGEX--
Fatal error: Allowed memory size of \d+ bytes exhausted.*#\d+ \{main\}

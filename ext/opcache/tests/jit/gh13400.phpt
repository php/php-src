--TEST--
GH-13400: JIT code generated after fork is visible to the parent
--DESCRIPTION--
OPcache metadata and generated JIT code are shared by forked workers, so the
JIT mapping and its allocation pointer must remain shared after fork. This test
records the available JIT space before forking and makes only the child call a
function often enough to generate code. The parent then verifies that it sees
the reduced free space and can execute the generated function.

Without shared inheritance, a private JIT mapping becomes copy-on-write in the
child. The parent's free-space check would print bool(false), while shared
OPcache metadata could point the parent at code bytes that exist only in the
child and cause a crash instead of printing int(42). A startup-only test would
not detect this failure in fork-based SAPIs such as FPM or Apache.
--EXTENSIONS--
opcache
pcntl
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_buffer_size=16M
opcache.jit_hot_func=1
--SKIPIF--
<?php
if (!function_exists('pcntl_fork')) die('skip pcntl_fork() not available');
if (!(opcache_get_status()['jit']['on'] ?? false)) die('skip JIT is not available');
?>
--FILE--
<?php
function value(): int {
    return 42;
}

$bufferFree = opcache_get_status()['jit']['buffer_free'];
$pid = pcntl_fork();
if ($pid === 0) {
    for ($i = 0; $i < 100; $i++) {
        value();
    }
    exit(0);
}
if ($pid === -1) {
    echo "pcntl_fork() failed\n";
    exit(1);
}

pcntl_waitpid($pid, $status);
var_dump(opcache_get_status()['jit']['buffer_free'] < $bufferFree);
var_dump(value());
?>
--EXPECT--
bool(true)
int(42)

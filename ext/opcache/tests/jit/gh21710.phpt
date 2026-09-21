--TEST--
GH-21710: tracing JIT side-trace compile with a heap-copied linked method
--EXTENSIONS--
opcache
pcntl
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_buffer_size=64M
--ENV--
call_user_func=call_user_func
--SKIPIF--
<?php
if (!function_exists('pcntl_fork')) die('skip pcntl_fork() not available');
if (!(opcache_get_status()['jit']['on'] ?? false)) die('skip JIT is not available');
?>
--FILE--
<?php
$pid = pcntl_fork();
if ($pid === 0) {
    require __DIR__ . '/gh21710.inc';
    for ($i = 0; $i < 1000; $i++) {
        getenv('call_user_func')('C::f', [false]);
    }
    exit(0);
}
if ($pid === -1) {
    echo "pcntl_fork() failed\n";
    exit(1);
}

pcntl_waitpid($pid, $status, 0);

$buf = [];
for ($i = 0; $i < 100; $i++) {
    $buf[] = str_repeat('a', $i * 100);
}

require __DIR__ . '/gh21710.inc';

for ($i = 0; $i < 1000; $i++) {
    getenv('call_user_func')('C::f', [true]);
}

var_dump(getenv('call_user_func')('C::f', [true]));
?>
--EXPECT--
bool(true)

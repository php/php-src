--TEST--
GH-17990 (Tracing JIT uses the SSA of a process that died during trace compilation)
--EXTENSIONS--
opcache
pcntl
posix
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.jit_hot_func=1
--FILE--
<?php
function foo($x) {
    return $x + 42;
}

function test() {
    return foo(3);
}

/* Kill a child at different moments, so that one of them dies while it compiles
 * the trace of test(). The shutdown function starts that trace outside of any
 * other trace. */
for ($delay = 0; $delay <= 2000; $delay += 50) {
    $pid = pcntl_fork();
    if ($pid === -1) {
        die("fork failed\n");
    }
    if ($pid === 0) {
        register_shutdown_function('test');
        exit(0);
    }
    usleep($delay);
    posix_kill($pid, SIGKILL);
    pcntl_waitpid($pid, $status);
}

register_shutdown_function(function () {
    var_dump(test());
});
?>
--EXPECT--
int(45)

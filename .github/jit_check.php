<?php

register_shutdown_function(function () {
    $status = opcache_get_status(false);
    var_dump($status);

    if ($status["memory_usage"]["free_memory"] < 10*1024*1024) {
        fwrite(STDERR, "Not enough free opcache memory!".PHP_EOL);
    }
    if ($status["interned_strings_usage"]["free_memory"] < 1*1024*1024) {
        fwrite(STDERR, "Not enough free interned strings memory!".PHP_EOL);
    }
    if ($status["jit"]["buffer_free"] < 10*1024*1024) {
        fwrite(STDERR, "Not enough free JIT memory!".PHP_EOL);
    }
    if (!$status["jit"]["on"]) {
        fwrite(STDERR, "JIT is not enabled!".PHP_EOL);
    }

    unset($status);
    while (gc_collect_cycles());
});

$argc--;
array_shift($argv);

$_SERVER['argc']--;
array_shift($_SERVER['argv']);

require $argv[0];

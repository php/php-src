<?php

$code = '<?php

register_shutdown_function(function () {
    $status = opcache_get_status();
    var_dump($status);

    if ($status["memory_usage"]["free_memory"] < 10*1024) {
        echo "Not enough free opcache memory!".PHP_EOL;
        die(130);
    }
    if ($status["interned_strings_usage"]["free_memory"] < 1*1024) {
        echo "Not enough free interned strings memory!".PHP_EOL;
        die(130);
    }
    if ($status["jit"]["buffer_free"] < 10*1024) {
        echo "Not enough free JIT memory!".PHP_EOL;
        die(130);
    }

    gc_collect_cycles();
});';

file_put_contents(
    $argv[1],
    str_replace('<?php', $code, file_get_contents($argv[1]))
);

--TEST--
php-cgi -b rejects ports outside 0-65535
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

include "include.inc";

$php = get_cgi_path();
reset_env_vars();

foreach (['127.0.0.1:70000', '127.0.0.1:-1', '70000'] as $bind) {
    $proc = proc_open([$php, '-n', '-b', $bind], [1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
    for ($i = 0; $i < 50 && proc_get_status($proc)['running']; $i++) {
        usleep(100000);
    }
    if (proc_get_status($proc)['running']) {
        proc_terminate($proc);
        echo "$bind: still listening\n";
    } else {
        echo trim(stream_get_contents($pipes[2])), "\n";
    }
    proc_close($proc);
}

?>
--EXPECT--
Couldn't create FastCGI listen socket on port 127.0.0.1:70000
Couldn't create FastCGI listen socket on port 127.0.0.1:-1
Couldn't create FastCGI listen socket on port 70000

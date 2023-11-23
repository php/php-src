--TEST--
proc_open() with socket and pipe
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) {
    // finished in 1.15s in a best-of-3 on an idle Intel Xeon X5670 on PHP 8.3.0-dev
    die("skip slow test");
}
?>
--FILE--
<?php

function poll($pipe, $read = true)
{
    $r = ($read == true) ? [$pipe] : null;
    $w = ($read == false) ? [$pipe] : null;
    $e = null;

    if (!stream_select($r, $w, $e, null)) {
        throw new \Error("Select failed");
    }
}

function read_pipe($pipe): string
{
    poll($pipe);

    if (false === ($chunk = @fread($pipe, 8192))) {
        throw new Error("Failed to read: " . (error_get_last()['message'] ?? 'N/A'));
    }

    return $chunk;
}

$cmd = [
    getenv("TEST_PHP_EXECUTABLE"),
    __DIR__ . '/proc_open_sockets2.inc'
];

$spec = [
    ['pipe', 'r'],
    ['socket']
];

$proc = proc_open($cmd, $spec, $pipes);

var_dump(stream_set_blocking($pipes[1], false));

printf("STDOUT << %s\n", read_pipe($pipes[1]));
printf("STDOUT << %s\n", read_pipe($pipes[1]));

fwrite($pipes[0], 'done');
fclose($pipes[0]);

printf("STDOUT << %s\n", read_pipe($pipes[1]));

?>
--EXPECT--
bool(true)
STDOUT << hello
STDOUT << world
STDOUT << DONE

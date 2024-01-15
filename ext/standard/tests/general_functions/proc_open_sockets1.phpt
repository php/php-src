--TEST--
proc_open() with output socketpairs
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("SKIP_SLOW_TESTS");
?>
--FILE--
<?php

$cmd = [
    getenv("TEST_PHP_EXECUTABLE"),
    __DIR__ . '/proc_open_sockets1.inc'
];

$spec = [
    ['null'],
    ['socket'],
    ['socket']
];

$proc = proc_open($cmd, $spec, $pipes);

foreach ($pipes as $pipe) {
    var_dump(stream_set_blocking($pipe, false));
}

while ($pipes) {
    $r = $pipes;
    $w = null;
    $e = null;

    if (!stream_select($r, $w, $e, null)) {
        throw new Error("Select failed");
    }

    foreach ($r as $i => $pipe) {
        if (!is_resource($pipe) || feof($pipe)) {
            unset($pipes[$i]);
            continue;
        }

        $chunk = @fread($pipe, 8192);

        if ($chunk === false) {
            throw new Error("Failed to read: " . (error_get_last()['message'] ?? 'N/A'));
        }

        if ($chunk !== '') {
            echo "PIPE {$i} << {$chunk}\n";
        }
    }
}

?>
--EXPECT--
bool(true)
bool(true)
PIPE 1 << hello
PIPE 2 << SOME ERROR
PIPE 1 << world

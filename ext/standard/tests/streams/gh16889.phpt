--TEST--
GH-16889 (stream_select() timeout useless for pipes on Windows)
--FILE--
<?php
$desc = [
    ["pipe", "r"],
    ["pipe", "w"],
    ["pipe", "w"],
];
// open process which won't produce output for 10s
$proc = proc_open([PHP_BINARY, "-r", "sleep(10); echo 'finish';"], $desc, $pipes);
$read = [$pipes[1]];
$write = null;
$except = null;
$time0 = microtime(true);
// select STDOUT pipe of process for 1ms
if (stream_select($read, $write, $except, 0, 1000)) {
    var_dump(fread($read[0], 1));
}
// avoid blocking of finishing the test process
proc_terminate($proc);
$time1 = microtime(true);
var_dump($time1 - $time0 < 1);
?>
--EXPECT--
bool(true)

--TEST--
Casting a stream can lose data and needs to emit a warning
--SKIPIF--
<?php
if (PHP_OS_FAMILY === "Windows") die("skip non-Windows tests (popen cannot delete file as open in cmd.exe)");
?>
--FILE--
<?php

$tempnam = tempnam(sys_get_temp_dir(), 'test');

$stream = popen('echo 1; echo 2; rm ' . escapeshellarg($tempnam), 'r');

do {
    usleep(10);
    clearstatcache();
} while (file_exists($tempnam));

// fills the read buffer with up to 8192 bytes
fgets($stream);

// cast $stream and read fd until eof. Print each line that was read, prefixed with "proc open stdin:"
$process = proc_open(
    'sed "s/^/proc open stdin:/"',
    [
        0 => $stream,
        1 => ['pipe', 'w'],
    ],
    $pipes,
);

var_dump(stream_get_contents($pipes[1]));

?>
--EXPECTF--
Warning: proc_open(): 2 bytes of buffered data lost during stream conversion! in %s on line %d
string(0) ""

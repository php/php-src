--TEST--
Io\Terminal\Terminal: readLine nonblocking TTY EAGAIN waiting, partial lines, and EOF contracts
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    die("skip PTY test is POSIX-only");
}
if (!function_exists('proc_open')) {
    die("skip proc_open is not available");
}
try {
    $proc = @proc_open(
        [PHP_BINARY, '-r', ''],
        [
            0 => ['pty'],
            1 => ['pty'],
            2 => ['pipe', 'w'],
        ],
        $pipes,
    );
} catch (Throwable) {
    die("skip PTY is not available");
}
if (!is_resource($proc)) {
    die("skip PTY is not available");
}
foreach ($pipes as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc);
?>
--FILE--
<?php

use Io\Terminal\Terminal;

// 1. Nonblocking TTY, no data initially: must not become EOF/null
$proc1 = proc_open(
    [PHP_BINARY, '-r', '
        fwrite(STDERR, "STARTED\n");
        fflush(STDERR);
        fgets(STDIN);
        usleep(40000);
        fwrite(STDOUT, "delayed line\n");
        fflush(STDOUT);
    '],
    [
        0 => ['pipe', 'r'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes1,
);

fgets($pipes1[2]); // Wait for STARTED
stream_set_blocking($pipes1[1], false);
$terminal1 = Terminal::fromStreams($pipes1[1]);

fwrite($pipes1[0], "GO\n");
var_dump($terminal1->readLine());

fclose($pipes1[0]);
fclose($pipes1[1]);
fclose($pipes1[2]);
proc_close($proc1);

// 2. Nonblocking TTY, partial bytes without newline: must not return them as a completed line
$proc2 = proc_open(
    [PHP_BINARY, '-r', '
        fwrite(STDERR, "STARTED\n");
        fflush(STDERR);
        fgets(STDIN);
        fwrite(STDOUT, "partial_");
        fflush(STDOUT);
        usleep(40000);
        fwrite(STDOUT, "completed\n");
        fflush(STDOUT);
    '],
    [
        0 => ['pipe', 'r'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes2,
);

fgets($pipes2[2]); // Wait for STARTED
stream_set_blocking($pipes2[1], false);
$terminal2 = Terminal::fromStreams($pipes2[1]);

fwrite($pipes2[0], "GO\n");
var_dump($terminal2->readLine());

fclose($pipes2[0]);
fclose($pipes2[1]);
fclose($pipes2[2]);
proc_close($proc2);

// 3. Actual EOF before data on nonblocking TTY returns null
$proc3 = proc_open(
    [PHP_BINARY, '-r', 'exit(0);'],
    [
        0 => ['pipe', 'r'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes3,
);

fclose($pipes3[0]);
fclose($pipes3[2]);
usleep(30000); // Allow child to exit and slave to close
stream_set_blocking($pipes3[1], false);
$terminal3 = Terminal::fromStreams($pipes3[1]);

var_dump($terminal3->readLine());

fclose($pipes3[1]);
proc_close($proc3);

// 4. Partial bytes + actual EOF on nonblocking TTY returns final unterminated line, then null
$proc4 = proc_open(
    [PHP_BINARY, '-r', '
        fwrite(STDERR, "STARTED\n");
        fflush(STDERR);
        fgets(STDIN);
        fwrite(STDOUT, "final unterminated");
        fflush(STDOUT);
        exit(0);
    '],
    [
        0 => ['pipe', 'r'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes4,
);

fgets($pipes4[2]); // Wait for STARTED
stream_set_blocking($pipes4[1], false);
$terminal4 = Terminal::fromStreams($pipes4[1]);

fwrite($pipes4[0], "GO\n");
usleep(30000); // Allow child to write and exit
var_dump($terminal4->readLine());
var_dump($terminal4->readLine()); // Immediate EOF after unterminated line

fclose($pipes4[0]);
fclose($pipes4[1]);
fclose($pipes4[2]);
proc_close($proc4);

?>
--EXPECT--
string(12) "delayed line"
string(17) "partial_completed"
NULL
string(18) "final unterminated"
NULL

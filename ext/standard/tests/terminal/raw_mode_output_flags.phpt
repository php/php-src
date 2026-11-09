--TEST--
Io\Terminal\Terminal: c_oflag is preserved when entering raw mode with OPOST on and off
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    die("skip POSIX termios test");
}
if (!function_exists('proc_open')) {
    die("skip proc_open not available");
}
try {
    $proc = @proc_open(
        [PHP_BINARY, '-r', ''],
        [0 => ['pty'], 1 => ['pty'], 2 => ['pipe', 'w']],
        $pipes
    );
} catch (Throwable) {
    die("skip PTY not available");
}
if (!is_resource($proc)) {
    die("skip PTY not available");
}
foreach ($pipes as $p) { if (is_resource($p)) fclose($p); }
proc_close($proc);
?>
--FILE--
<?php

use Io\Terminal\Terminal;

function getOpostState($stream): bool {
    $sub = proc_open(['stty', '-a'], [0 => $stream, 1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
    if (!is_resource($sub)) {
        return false;
    }
    $out = stream_get_contents($pipes[1]);
    fclose($pipes[1]);
    fclose($pipes[2]);
    proc_close($sub);
    $hasOpost = preg_match('/(?:\s|^)opost(?:\s|$)/', $out) === 1;
    $hasMinusOpost = preg_match('/(?:\s|^)-opost(?:\s|$)/', $out) === 1;
    return $hasOpost && !$hasMinusOpost;
}

function setOpostState($stream, bool $enable): void {
    $cmd = $enable ? ['stty', 'opost'] : ['stty', '-opost'];
    $sub = proc_open($cmd, [0 => $stream, 1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
    if (is_resource($sub)) {
        fclose($pipes[1]);
        fclose($pipes[2]);
        proc_close($sub);
    }
}

// Case 1: OPOST is initially ON
$proc1 = proc_open(
    [PHP_BINARY, '-r', 'fgets(STDIN);'],
    [0 => ['pty'], 1 => ['pty'], 2 => ['pipe', 'w']],
    $pipes1
);
$term1 = Terminal::fromStreams($pipes1[0]);

$before1 = getOpostState($pipes1[0]);
$token1 = $term1->enableRawMode();
$during1 = getOpostState($pipes1[0]);
$term1->restoreMode($token1);
$after1 = getOpostState($pipes1[0]);

// OPOST was ON before, must stay ON during raw mode and after restore
var_dump($before1 === true);
var_dump($during1 === true);
var_dump($after1 === true);

fwrite($pipes1[0], "\n");
fclose($pipes1[0]);
if (isset($pipes1[1]) && is_resource($pipes1[1])) fclose($pipes1[1]);
if (isset($pipes1[2]) && is_resource($pipes1[2])) fclose($pipes1[2]);
proc_terminate($proc1);
proc_close($proc1);

// Case 2: OPOST is initially OFF
$proc2 = proc_open(
    [PHP_BINARY, '-r', 'fgets(STDIN);'],
    [0 => ['pty'], 1 => ['pty'], 2 => ['pipe', 'w']],
    $pipes2
);
$term2 = Terminal::fromStreams($pipes2[0]);

// Turn OPOST OFF explicitly before acquiring raw mode
setOpostState($pipes2[0], false);

$before2 = getOpostState($pipes2[0]);
$token2 = $term2->enableRawMode();
$during2 = getOpostState($pipes2[0]);
$term2->restoreMode($token2);
$after2 = getOpostState($pipes2[0]);

// OPOST was OFF before, must NOT be forced ON during raw mode, and remain OFF after restore
var_dump($before2 === false);
var_dump($during2 === false);
var_dump($after2 === false);

fwrite($pipes2[0], "\n");
fclose($pipes2[0]);
if (isset($pipes2[1]) && is_resource($pipes2[1])) fclose($pipes2[1]);
if (isset($pipes2[2]) && is_resource($pipes2[2])) fclose($pipes2[2]);
proc_terminate($proc2);
proc_close($proc2);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

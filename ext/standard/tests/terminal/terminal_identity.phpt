--TEST--
Io\Terminal\Terminal: POSIX terminal identity and unrelated PTY rejection
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
use Io\Terminal\ModeToken;

$proc1 = proc_open(
    [PHP_BINARY, '-r', 'fgets(STDIN);'],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes1,
);

$proc2 = proc_open(
    [PHP_BINARY, '-r', 'fgets(STDIN);'],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes2,
);

$t1 = Terminal::fromStreams($pipes1[0]);
$t1_dup = Terminal::fromStreams($pipes1[1]);
$t2 = Terminal::fromStreams($pipes2[0]);

$m1_dup = $t1->enableRawMode();
var_dump($m1_dup instanceof ModeToken);
var_dump($t1_dup->restoreMode($m1_dup));

$m1 = $t1->enableRawMode();
try {
    $t2->restoreMode($m1);
    echo "FAIL: unrelated PTY accepted token\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

var_dump($t1->restoreMode($m1));
var_dump($t1->restoreMode());

fwrite($pipes1[0], "exit\n");
fwrite($pipes2[0], "exit\n");
unset($t1, $t1_dup, $t2);
foreach ($pipes1 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
foreach ($pipes2 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc1);
proc_close($proc2);
?>
--EXPECT--
bool(true)
bool(true)
ValueError: Io\Terminal\Terminal::restoreMode(): Argument #1 ($mode) must be an active terminal mode token belonging to this terminal
bool(true)
bool(false)

--TEST--
Io\Terminal\Terminal: overlapping raw-mode sessions and out-of-order restoration
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

$proc = proc_open(
    [PHP_BINARY, '-r', 'fgets(STDIN);'],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes,
);

// Two separate Terminal instances referencing descriptors on the same PTY
$t1 = Terminal::fromStreams($pipes[0]);
$t2 = Terminal::fromStreams($pipes[1]);

// Test 1: Out-of-order restore (m1 then m2)
$m1 = $t1->enableRawMode();
$m2 = $t2->enableRawMode();

var_dump($m1 instanceof ModeToken);
var_dump($m2 instanceof ModeToken);

// Out-of-order: restore older token m1 first
var_dump($t1->restoreMode($m1));

// Now restore m2
var_dump($t2->restoreMode($m2));

// Test 2: In-order restore (m2 then m1)
$m1 = $t1->enableRawMode();
$m2 = $t2->enableRawMode();

var_dump($t2->restoreMode($m2));
var_dump($t1->restoreMode($m1));

fwrite($pipes[0], "exit\n");
unset($t1, $t2);
foreach ($pipes as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

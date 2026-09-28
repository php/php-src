--TEST--
Io\Terminal\Terminal: readKey zero timeout returns Escape immediately without sequence delay
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

use Io\Terminal\Key;
use Io\Terminal\Terminal;
use Time\Duration;

$code = 'fgets(STDIN); fwrite(STDOUT, "\x1b"); fflush(STDOUT); fwrite(STDERR, "READY\n"); fflush(STDERR); fgets(STDIN);';
$proc = proc_open(
    [PHP_BINARY, '-r', $code],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes,
);

$terminal = Terminal::fromStreams($pipes[0]);
$terminal->enableRawMode();

// Signal child to write lone escape
fwrite($pipes[0], "GO\n");
$ready = fgets($pipes[2]);

// Read with zero timeout (non-blocking)
$start = hrtime(true);
$key = $terminal->readKey(Duration::fromSeconds(0));
$elapsed_ms = (hrtime(true) - $start) / 1e6;

var_dump($key === Key::Escape);
// Verify it did not block for 25ms sequence timeout (should be < 15ms)
var_dump($elapsed_ms < 15.0);

fwrite($pipes[0], "DONE\n");
unset($terminal);
foreach ($pipes as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc);
?>
--EXPECT--
bool(true)
bool(true)

--TEST--
Io\Terminal\Terminal: readKey null vs TerminalException contracts on POSIX PTY
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
use Time\Duration;

$proc = proc_open(
    [PHP_BINARY, '-r', 'fgets(STDIN);'],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes,
);

$terminal = Terminal::fromStreams($pipes[0]);
$terminal->enableRawMode();

// Contract 1: No input + zero timeout (non-blocking) returns null immediately
$start = hrtime(true);
$key = $terminal->readKey(Duration::fromSeconds(0));
$elapsed_ms = (hrtime(true) - $start) / 1e6;

var_dump($key === null);
var_dump($elapsed_ms < 20.0);

// Contract 2: No input + small finite timeout returns null after duration
$start = hrtime(true);
$key = $terminal->readKey(Duration::fromMilliseconds(25));
$elapsed_ms = (hrtime(true) - $start) / 1e6;

var_dump($key === null);
var_dump($elapsed_ms >= 20.0);

// Contract 3: Negative Duration throws ValueError
try {
    $terminal->readKey(Duration::fromSeconds(1)->negate());
    echo "FAIL: negative timeout accepted
";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

try {
    $terminal->readKey(Duration::fromSeconds(0), Duration::fromSeconds(1)->negate());
    echo "FAIL: negative sequence timeout accepted
";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

// Contract 4: Non-terminal input stream reads keys normally
$fp = fopen('php://temp', 'r+');
fwrite($fp, "z");
rewind($fp);
$nonTty = Terminal::fromStreams($fp);
$keyNonTty = $nonTty->readKey(Duration::fromSeconds(0));
var_dump($keyNonTty === "z");
fclose($fp);

// Contract 5: EOF / stream termination throws TerminalException on real TTY
fwrite($pipes[0], "exit
");
usleep(50000); // give child time to exit

try {
    $terminal->readKey(Duration::fromSeconds(1));
    echo "FAIL: readKey on closed/EOF terminal did not throw
";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

unset($terminal);
foreach ($pipes as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc);
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
ValueError: Io\Terminal\Terminal::readKey(): Argument #1 ($timeout) must not be negative
ValueError: Io\Terminal\Terminal::readKey(): Argument #2 ($sequenceTimeout) must not be negative
bool(true)
Io\Terminal\TerminalException: %s terminal input stream

--TEST--
Io\Terminal\Terminal: enableRawMode, restoreMode, session mode tracking and stale token protection
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

$terminal = Terminal::fromStreams($pipes[0]);

// Explicit token restore on valid terminal
$token = $terminal->enableRawMode();
echo "enableRawMode: ", ($token instanceof ModeToken ? "ModeToken" : "false"), "\n";

// Restore explicitly with token
var_dump($terminal->restoreMode($token));

// Stale consumed token must throw ValueError
try {
    $terminal->restoreMode($token);
    echo "FAIL: stale token was accepted\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

// Session-managed restore without args
$token2 = $terminal->enableRawMode();
var_dump($token2 instanceof ModeToken);
var_dump($terminal->restoreMode()); // Restores active token
var_dump($terminal->restoreMode()); // Already restored -> returns false

// Auto-restoration on Terminal object destruction
$terminal2 = Terminal::fromStreams($pipes[0]);
$token3 = $terminal2->enableRawMode();
var_dump($token3 instanceof ModeToken);
unset($terminal2); // Terminal destructor releases active mode token

// Restoring with the token from the destroyed terminal must throw ValueError
try {
    $terminal->restoreMode($token3);
    echo "FAIL: token from destroyed terminal was accepted\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

fwrite($pipes[0], "exit\n");
unset($terminal, $token3);
foreach ($pipes as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc);
?>
--EXPECTF--
enableRawMode: ModeToken
bool(true)
ValueError: Io\Terminal\Terminal::restoreMode(): Argument #1 ($mode) must be an active terminal mode token returned by Io\Terminal\Terminal::enableRawMode()
bool(true)
bool(true)
bool(false)
bool(true)
ValueError: Io\Terminal\Terminal::restoreMode(): Argument #1 ($mode) must be an active terminal mode token returned by Io\Terminal\Terminal::enableRawMode()

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

// Auto-restoration on Terminal object destruction when token is unreferenced
$terminal2 = Terminal::fromStreams($pipes[0]);
$terminal2->enableRawMode();
unset($terminal2); // Terminal destructor drops reference; ModeToken destructor restores mode

// Token survives Terminal object destruction when held externally
$terminal3 = Terminal::fromStreams($pipes[0]);
$token3 = $terminal3->enableRawMode();
var_dump($token3 instanceof ModeToken);
unset($terminal3); // Drops Terminal reference, but $token3 is held externally

// Restoring the externally held token through another Terminal on the same terminal succeeds
var_dump($terminal->restoreMode($token3));

// Once restored, reusing the token throws ValueError
try {
    $terminal->restoreMode($token3);
    echo "FAIL: consumed token was accepted\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

// Closing the original PHP stream after enableRawMode() does NOT prevent successful restoration
// because the shared record owns its own duplicated restoration descriptor
$proc2 = proc_open(
    [PHP_BINARY, '-r', 'fgets(STDIN);'],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes2,
);
$terminalClosed = Terminal::fromStreams($pipes2[0]);
$tokenClosed = $terminalClosed->enableRawMode();
var_dump($tokenClosed instanceof ModeToken);

// Close the underlying stream
fclose($pipes2[0]);

// Restoration succeeds via the record's owned descriptor
var_dump($terminalClosed->restoreMode($tokenClosed));

// Token is now consumed; reusing it throws ValueError
try {
    $terminalClosed->restoreMode($tokenClosed);
    echo "FAIL: consumed token was accepted\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

// Further restoreMode() returns false
var_dump($terminalClosed->restoreMode());

proc_terminate($proc2);
foreach ($pipes2 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc2);

fwrite($pipes[0], "exit\n");
unset($terminal, $token3, $terminalClosed, $tokenClosed);
foreach ($pipes as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc);
?>
--EXPECT--
enableRawMode: ModeToken
bool(true)
ValueError: Io\Terminal\Terminal::restoreMode(): Argument #1 ($mode) must be an active terminal mode token belonging to this terminal
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
ValueError: Io\Terminal\Terminal::restoreMode(): Argument #1 ($mode) must be an active terminal mode token belonging to this terminal
bool(true)
bool(true)
ValueError: Io\Terminal\Terminal::restoreMode(): Argument #1 ($mode) must be an active terminal mode token belonging to this terminal
bool(false)

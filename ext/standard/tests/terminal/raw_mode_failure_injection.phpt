--TEST--
Io\Terminal\Terminal: raw mode acquisition and restoration failure rollback and retry
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    die("skip POSIX PTY test");
}
if (!defined('PHP_DEBUG') || !PHP_DEBUG) {
    die("skip debug build required for test seam");
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
use Io\Terminal\TerminalException;

function isPtyRaw($stream): bool {
    $sub = proc_open(['stty', '-a'], [0 => $stream, 1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
    if (!is_resource($sub)) return false;
    $out = stream_get_contents($pipes[1]);
    fclose($pipes[1]); fclose($pipes[2]); proc_close($sub);
    return strpos($out, "-icanon") !== false && strpos($out, "-echo") !== false;
}

$proc = proc_open(
    [PHP_BINARY, '-r', 'fgets(STDIN);'],
    [0 => ['pty'], 1 => ['pty'], 2 => ['pipe', 'w']],
    $pipes
);
$term = Terminal::fromStreams($pipes[0]);

// Test 1: Reapplication failure must not leak lease count
$m1 = $term->enableRawMode();
var_dump(isPtyRaw($pipes[0]));

putenv("PHP_TERMINAL_TEST_SEAM=fail_reapply");
try {
    $m2 = $term->enableRawMode();
    echo "FAIL: reapplication succeeded despite failure seam\n";
} catch (Throwable $e) {
    echo "PASS: reapplication rejected: " . $e->getMessage() . "\n";
}
putenv("PHP_TERMINAL_TEST_SEAM");

// Because reapplication failed, lease count was NOT incremented.
// Restoring $m1 must therefore restore canonical mode immediately!
$term->restoreMode($m1);
var_dump(!isPtyRaw($pipes[0]));

// Test 2: Failed explicit restoration leaves retryable token
$mRetry = $term->enableRawMode();
var_dump(isPtyRaw($pipes[0]));

putenv("PHP_TERMINAL_TEST_SEAM=fail_restore_once");
try {
    $term->restoreMode($mRetry);
    echo "FAIL: restore succeeded despite failure seam\n";
} catch (TerminalException $e) {
    echo "PASS: restore threw: " . $e->getMessage() . "\n";
}
putenv("PHP_TERMINAL_TEST_SEAM");

// Token must still be active and terminal still in raw mode
var_dump(isPtyRaw($pipes[0]));

// Retry restoration succeeds and restores mode
$term->restoreMode($mRetry);
var_dump(!isPtyRaw($pipes[0]));

// Third restore attempt fails because token was consumed
try {
    $term->restoreMode($mRetry);
    echo "FAIL: consumed token accepted on retry\n";
} catch (ValueError $e) {
    echo "PASS: token consumed on successful retry\n";
}

// Test 3: Postcondition failure rolls back initial mode
putenv("PHP_TERMINAL_TEST_SEAM=fail_postcondition");
try {
    $mFail = $term->enableRawMode();
    echo "FAIL: acquisition succeeded despite postcondition failure\n";
} catch (Throwable $e) {
    echo "PASS: postcondition failure caught: " . $e->getMessage() . "\n";
}
putenv("PHP_TERMINAL_TEST_SEAM");
// Terminal must NOT be left in raw mode after failed acquisition
var_dump(!isPtyRaw($pipes[0]));

fwrite($pipes[0], "\n");
fclose($pipes[0]);
if (isset($pipes[1]) && is_resource($pipes[1])) fclose($pipes[1]);
if (isset($pipes[2]) && is_resource($pipes[2])) fclose($pipes[2]);
proc_terminate($proc);
proc_close($proc);

?>
--EXPECTF--
bool(true)
PASS: reapplication rejected: %s
bool(true)
bool(true)
PASS: restore threw: Failed to restore terminal mode
bool(true)
bool(true)
PASS: token consumed on successful retry
PASS: postcondition failure caught: %s
bool(true)

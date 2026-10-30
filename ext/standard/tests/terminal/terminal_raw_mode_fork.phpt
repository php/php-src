--TEST--
Io\Terminal\Terminal: raw mode token ownership and child process fork isolation
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    die("skip POSIX pcntl_fork test");
}
if (!function_exists('pcntl_fork')) {
    die("skip pcntl_fork not available");
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

$proc = proc_open(
    [PHP_BINARY, '-r', 'fgets(STDIN);'],
    [0 => ['pty'], 1 => ['pty'], 2 => ['pipe', 'w']],
    $pipes
);

$term = Terminal::fromStreams($pipes[0]);

function isPtyRaw($stream): bool {
    $sub = proc_open(['stty', '-a'], [0 => $stream, 1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
    if (!is_resource($sub)) return false;
    $out = stream_get_contents($pipes[1]);
    fclose($pipes[1]); fclose($pipes[2]); proc_close($sub);
    return strpos($out, "-icanon") !== false && strpos($out, "-echo") !== false;
}

// 1. Parent enables raw mode
$token = $term->enableRawMode();
var_dump($token !== null);

// 2. Fork child
$pid = pcntl_fork();
if ($pid === -1) {
    die("Failed to fork");
}

if ($pid === 0) {
    // Child process:
    // A. Explicit restore by child must be rejected
    try {
        $term->restoreMode($token);
        echo "CHILD_RESTORE_FAIL: restore succeeded in child\n";
    } catch (\Throwable $e) {
        echo "CHILD_RESTORE_PASS: " . $e->getMessage() . "\n";
    }

    // B. Child unsets inherited token and terminates
    unset($token);
    exit(0);
}

// Parent process waits for child to exit
pcntl_waitpid($pid, $status);

// Verify actual OS mode after child exit and before parent restoration
var_dump(isPtyRaw($pipes[0]));

// 3. Parent verifies its token is still active and can be restored
try {
    $term->restoreMode($token);
    echo "PARENT_RESTORE_PASS\n";
} catch (\Throwable $e) {
    echo "PARENT_RESTORE_FAIL: " . $e->getMessage() . "\n";
}

// Verify actual OS mode after parent restoration
var_dump(!isPtyRaw($pipes[0]));

// 4. Token cannot be used a second time
try {
    $term->restoreMode($token);
    echo "REUSE_FAIL\n";
} catch (\Throwable $e) {
    echo "REUSE_PASS\n";
}

fwrite($pipes[0], "\n");
fclose($pipes[0]);
if (isset($pipes[1]) && is_resource($pipes[1])) fclose($pipes[1]);
if (isset($pipes[2]) && is_resource($pipes[2])) fclose($pipes[2]);
proc_terminate($proc);
proc_close($proc);

?>
--EXPECTF--
bool(true)
CHILD_RESTORE_PASS: %smust be an active terminal mode token belonging to this terminal
bool(true)
PARENT_RESTORE_PASS
bool(true)
REUSE_PASS

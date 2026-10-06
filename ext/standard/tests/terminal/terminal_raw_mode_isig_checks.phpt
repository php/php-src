--TEST--
Io\Terminal\Terminal: raw mode configuration when ECHO and ICANON are disabled but ISIG is enabled
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    die("skip POSIX PTY test");
}
if (!function_exists('proc_open')) {
    die("skip proc_open not available");
}
try {
    $proc = @proc_open(
        [PHP_BINARY, '-r', ''],
        [0 => ['pty'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']],
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

// Bounded overall execution safety
if (function_exists('pcntl_alarm')) {
    pcntl_alarm(10);
}

function runIsolatedChild(string $code, ?string $inputToSend = null): string {
    $descriptors = [
        0 => ['pty'],
        1 => ['pipe', 'w'],
        2 => ['pipe', 'w'],
    ];

    $proc = proc_open([PHP_BINARY, '-r', $code], $descriptors, $pipes);
    if (!is_resource($proc)) {
        return "FAIL: proc_open failed";
    }

    // Readiness synchronization: wait for child to signal READY
    stream_set_timeout($pipes[1], 3);
    $ready = fgets($pipes[1]);
    if (trim($ready) !== 'READY') {
        foreach ($pipes as $p) { if (is_resource($p)) fclose($p); }
        proc_terminate($proc);
        proc_close($proc);
        return "FAIL: child not ready: " . var_export($ready, true);
    }

    if ($inputToSend !== null) {
        usleep(50000);
        fwrite($pipes[0], $inputToSend);
        fflush($pipes[0]);
    }

    // Read result
    $result = fgets($pipes[1]);
    $stderr = stream_get_contents($pipes[2]);

    foreach ($pipes as $p) { if (is_resource($p)) fclose($p); }
    proc_close($proc);

    if (!empty($stderr)) {
        return "STDERR: " . trim($stderr);
    }

    return trim($result);
}

// Child helper functions template
$setupHelper = '
function setSttyMode(): void {
    $sub = proc_open(["stty", "-echo", "-icanon", "isig"], [0 => STDIN, 1 => ["pipe", "w"], 2 => ["pipe", "w"]], $sp);
    if (is_resource($sub)) {
        fclose($sp[1]); fclose($sp[2]); proc_close($sub);
    }
}
function isPrecedingModeRestored(): bool {
    $sub = proc_open(["stty", "-a"], [0 => STDIN, 1 => ["pipe", "w"], 2 => ["pipe", "w"]], $sp);
    if (!is_resource($sub)) return false;
    $out = stream_get_contents($sp[1]);
    fclose($sp[1]); fclose($sp[2]); proc_close($sub);
    $hasIsig = preg_match("/(?:\s|^)isig(?:\s|$)/", $out) === 1 && preg_match("/(?:\s|^)-isig(?:\s|$)/", $out) !== 1;
    $hasMinusEcho = strpos($out, "-echo") !== false;
    $hasMinusIcanon = strpos($out, "-icanon") !== false;
    return $hasIsig && $hasMinusEcho && $hasMinusIcanon;
}
';

// Case 1: readKey() receives Ctrl+C as input byte and restores preceding mode
$codeKey = $setupHelper . '
use Io\Terminal\Terminal;
use Time\Duration;
if (function_exists("pcntl_alarm")) pcntl_alarm(5);
setSttyMode();
$t = Terminal::fromStdio();
fwrite(STDOUT, "READY\n");
fflush(STDOUT);
$key = $t->readKey(Duration::fromSeconds(2));
$restored = isPrecedingModeRestored();
echo ($key === "\x03" && $restored) ? "PASS: readKey received Ctrl+C and restored mode" : "FAIL: key=" . bin2hex((string)$key) . " restored=" . ($restored ? "1" : "0");
';

echo runIsolatedChild($codeKey, "\x03"), "\n";

// Case 2: readSecret() handles Ctrl+C through TerminalException and restores preceding mode
$codeSecret = $setupHelper . '
use Io\Terminal\Terminal;
use Io\Terminal\TerminalException;
use Time\Duration;
if (function_exists("pcntl_alarm")) pcntl_alarm(5);
setSttyMode();
$t = Terminal::fromStdio();
fwrite(STDOUT, "READY\n");
fflush(STDOUT);
$caught = false;
try {
    $t->readSecret(Duration::fromSeconds(2));
} catch (TerminalException $e) {
    $caught = true;
}
$restored = isPrecedingModeRestored();
echo ($caught && $restored) ? "PASS: readSecret threw TerminalException on Ctrl+C and restored mode" : "FAIL: caught=" . ($caught ? "1" : "0") . " restored=" . ($restored ? "1" : "0");
';

echo runIsolatedChild($codeSecret, "\x03"), "\n";

// Case 3: Shared-lease reapplication path applies raw mode when ISIG is enabled and restores afterward
$codeLease = $setupHelper . '
use Io\Terminal\Terminal;
if (function_exists("pcntl_alarm")) pcntl_alarm(5);
setSttyMode();
$term = Terminal::fromStdio();
fwrite(STDOUT, "READY\n");
fflush(STDOUT);
$t1 = $term->enableRawMode();

// Alter terminal mode back to -echo -icanon isig
setSttyMode();

// Reapply lease while first lease is still active
$t2 = $term->enableRawMode();

// Verify ISIG is now disabled in terminal
$sub = proc_open(["stty", "-a"], [0 => STDIN, 1 => ["pipe", "w"], 2 => ["pipe", "w"]], $sp);
$out = stream_get_contents($sp[1]);
fclose($sp[1]); fclose($sp[2]); proc_close($sub);
$hasMinusIsig = strpos($out, "-isig") !== false;

$term->restoreMode($t2);
$term->restoreMode($t1);

$restored = isPrecedingModeRestored();
echo ($hasMinusIsig && $restored) ? "PASS: lease reapplication disabled ISIG and restored mode" : "FAIL: raw=" . ($hasMinusIsig ? "1" : "0") . " restored=" . ($restored ? "1" : "0");
';

echo runIsolatedChild($codeLease, null), "\n";
?>
--EXPECT--
PASS: readKey received Ctrl+C and restored mode
PASS: readSecret threw TerminalException on Ctrl+C and restored mode
PASS: lease reapplication disabled ISIG and restored mode

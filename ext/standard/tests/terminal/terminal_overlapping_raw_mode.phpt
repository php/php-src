--TEST--
Io\Terminal\SystemTerminal: overlapping raw-mode sessions, ownership validation, and out-of-order restoration
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

use Io\Terminal\SystemTerminal;
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

$proc2 = proc_open(
    [PHP_BINARY, '-r', 'fgets(STDIN);'],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes2,
);

// Two separate Terminal instances referencing descriptors on the same PTY
$t1 = SystemTerminal::fromStreams($pipes[0]);
$t2 = SystemTerminal::fromStreams($pipes[1]);
$tUnrelated = SystemTerminal::fromStreams($pipes2[0]);

// Test 1: Same Terminal + token
$m = $t1->enableRawMode();
var_dump($m instanceof ModeToken);
var_dump($t1->restoreMode($m));

// Test 2: Different Terminal on same device
$m1 = $t1->enableRawMode();
var_dump($t2->restoreMode($m1));

// Test 3: Unrelated terminal rejection (ValueError)
$m1 = $t1->enableRawMode();
try {
    $tUnrelated->restoreMode($m1);
    echo "FAIL: unrelated terminal accepted token\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
var_dump($t1->restoreMode($m1));

// Test 4: Token survives Terminal destruction when held externally
$tTemp = SystemTerminal::fromStreams($pipes[0]);
$mTemp = $tTemp->enableRawMode();
unset($tTemp); // Drops Terminal reference, but $mTemp remains active

// $mTemp is still active and can be restored through another Terminal on the same device
var_dump($t1->restoreMode($mTemp));

// After restoration, the token is consumed; reusing it throws ValueError
try {
    $t1->restoreMode($mTemp);
    echo "FAIL: stale token after restoration accepted\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

// Test 5: Out-of-order restore (m1 then m2)
$m1 = $t1->enableRawMode();
$m2 = $t2->enableRawMode();

var_dump($m1 instanceof ModeToken);
var_dump($m2 instanceof ModeToken);

// Out-of-order: restore older token m1 first
var_dump($t1->restoreMode($m1));

// Now restore m2
var_dump($t2->restoreMode($m2));

// Test 6: In-order restore (m2 then m1)
$m1 = $t1->enableRawMode();
$m2 = $t2->enableRawMode();

var_dump($t2->restoreMode($m2));
var_dump($t1->restoreMode($m1));

// Test 7: Destroying the final externally-held token restores canonical mode
$tLive = SystemTerminal::fromStreams($pipes[0]);
$mHeld = $tLive->enableRawMode();
unset($tLive); // Terminal dropped; $mHeld keeps raw mode active
unset($mHeld); // Final token destroyed; ModeToken destructor restores canonical mode

// Terminal is back in canonical mode; further restore returns false
$tVerify = SystemTerminal::fromStreams($pipes[0]);
var_dump($tVerify->restoreMode());

fwrite($pipes[0], "exit\n");
fwrite($pipes2[0], "exit\n");
unset($t1, $t2, $tUnrelated, $tVerify);
foreach ($pipes as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
foreach ($pipes2 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc);
proc_close($proc2);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
ValueError: Io\Terminal\SystemTerminal::restoreMode(): Argument #1 ($mode) must be an active terminal mode token belonging to this terminal
bool(true)
bool(true)
ValueError: Io\Terminal\SystemTerminal::restoreMode(): Argument #1 ($mode) must be an active terminal mode token belonging to this terminal
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)

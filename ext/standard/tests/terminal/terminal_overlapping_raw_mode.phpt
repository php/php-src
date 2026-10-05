--TEST--
Io\Terminal\Terminal: overlapping raw-mode sessions, ownership validation, and out-of-order restoration
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

$proc2 = proc_open(
    [PHP_BINARY, '-r', 'fgets(STDIN);'],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes2,
);

// Helper to check actual OS termios raw mode (ICANON/ECHO disabled)
function isPtyRaw($stream): bool {
    $sub = proc_open(['stty', '-a'], [0 => $stream, 1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
    if (!is_resource($sub)) return false;
    $out = stream_get_contents($pipes[1]);
    fclose($pipes[1]); fclose($pipes[2]); proc_close($sub);
    return strpos($out, "-icanon") !== false && strpos($out, "-echo") !== false;
}

// Two separate Terminal instances referencing descriptors on the same PTY
$t1 = Terminal::fromStreams($pipes[0]);
$t2 = Terminal::fromStreams($pipes[1]);
$tUnrelated = Terminal::fromStreams($pipes2[0]);

// Test 1: Same Terminal + token
$m = $t1->enableRawMode();
var_dump($m instanceof ModeToken);
var_dump(isPtyRaw($pipes[0]));
$t1->restoreMode($m);
var_dump(!isPtyRaw($pipes[0]));
echo "restored m\n";

// Test 2: Different Terminal on same device
$m1 = $t1->enableRawMode();
$t2->restoreMode($m1);
echo "restored m1 via t2
";

// Test 3: Unrelated terminal rejection (ValueError)
$m1 = $t1->enableRawMode();
try {
    $tUnrelated->restoreMode($m1);
    echo "FAIL: unrelated terminal accepted token
";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
$t1->restoreMode($m1);
echo "restored m1 via t1
";

// Test 4: Token survives Terminal destruction when held externally
$tTemp = Terminal::fromStreams($pipes[0]);
$mTemp = $tTemp->enableRawMode();
unset($tTemp); // Drops Terminal reference, but $mTemp remains active

// $mTemp is still active and can be restored through another Terminal on the same device
$t1->restoreMode($mTemp);
echo "restored mTemp via t1
";

// After restoration, the token is consumed; reusing it throws ValueError
try {
    $t1->restoreMode($mTemp);
    echo "FAIL: stale token after restoration accepted
";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

// Test 5: Out-of-order restore (m1 then m2)
$m1 = $t1->enableRawMode();
$m2 = $t2->enableRawMode();

var_dump($m1 instanceof ModeToken);
var_dump($m2 instanceof ModeToken);
var_dump(isPtyRaw($pipes[0]));

// Out-of-order: restore older token m1 first
$t1->restoreMode($m1);
// Intermediate lease release: OS mode must remain raw
var_dump(isPtyRaw($pipes[0]));
echo "restored m1 out-of-order\n";

// Now restore m2 (final lease)
$t2->restoreMode($m2);
// Final lease release: OS mode must be restored to canonical
var_dump(!isPtyRaw($pipes[0]));
echo "restored m2 out-of-order\n";

// Test 6: In-order restore (m2 then m1)
$m1 = $t1->enableRawMode();
$m2 = $t2->enableRawMode();

$t2->restoreMode($m2);
echo "restored m2 in-order\n";
$t1->restoreMode($m1);
echo "restored m1 in-order\n";

// Test 7: Destroying the final externally-held token restores canonical mode
$tLive = Terminal::fromStreams($pipes[0]);
$mHeld = $tLive->enableRawMode();
var_dump(isPtyRaw($pipes[0]));
unset($tLive); // Terminal dropped; $mHeld keeps raw mode active
var_dump(isPtyRaw($pipes[0]));
unset($mHeld); // Final token destroyed; ModeToken destructor restores canonical mode
var_dump(!isPtyRaw($pipes[0]));

// Reusing consumed m1 throws ValueError
try {
    $t1->restoreMode($m1);
} catch (ValueError $e) {
    echo "Reusing consumed token caught: ", $e->getMessage(), "\n";
}

fwrite($pipes[0], "exit\n");
fwrite($pipes2[0], "exit\n");
unset($t1, $t2, $tUnrelated);
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
restored m
restored m1 via t2
ValueError: Io\Terminal\Terminal::restoreMode(): Argument #1 ($mode) must be an active terminal mode token belonging to this terminal
restored m1 via t1
restored mTemp via t1
ValueError: Io\Terminal\Terminal::restoreMode(): Argument #1 ($mode) must be an active terminal mode token belonging to this terminal
bool(true)
bool(true)
bool(true)
bool(true)
restored m1 out-of-order
bool(true)
restored m2 out-of-order
restored m2 in-order
restored m1 in-order
bool(true)
bool(true)
bool(true)
Reusing consumed token caught: Io\Terminal\Terminal::restoreMode(): Argument #1 ($mode) must be an active terminal mode token belonging to this terminal

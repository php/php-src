--TEST--
Io\Terminal\Terminal: getSize() does not suppress unreported Key::Resize notifications
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

$childCode = <<<'PHP'
$steps = [
    [30, 100, "READY1", null],
    [35, 120, "READY2", null],
    [40, 110, "READY3", null],
    [25, 90,  "READY4", null],
    [45, 130, "READY5", null],
    [32, 105, "READY6", "\x1b[A"],
];
foreach ($steps as [$rows, $cols, $tag, $payload]) {
    exec("stty rows {$rows} cols {$cols}");
    if ($payload !== null) {
        fwrite(STDOUT, $payload);
        fflush(STDOUT);
    }
    fwrite(STDERR, "{$tag}\n");
    fflush(STDERR);
    if (fgets(STDIN) === false) {
        break;
    }
}
PHP;

$proc = proc_open(
    [PHP_BINARY, '-r', $childCode],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes,
);

$zero = Duration::fromSeconds(0);

// 1. Establish initial baseline size A (100x30) via getSize()
fgets($pipes[2]);
$terminal = Terminal::fromStreams($pipes[0], $pipes[1]);
$token = $terminal->enableRawMode();

$sA = $terminal->getSize();
echo "baseline A: {$sA->cols}x{$sA->rows}\n";

// Resize to B (120x35), call getSize(), then verify readKey() still reports Key::Resize
fwrite($pipes[0], "next\n");
fgets($pipes[2]);

$sB = $terminal->getSize();
echo "getSize at B: {$sB->cols}x{$sB->rows}\n";
$kB1 = $terminal->readKey($zero);
echo "readKey after getSize at B: " . ($kB1 === Key::Resize ? 'Key::Resize' : var_export($kB1, true)) . "\n";

// 2. No repeated synthetic notification after the change has been reported
$kB2 = $terminal->readKey($zero);
echo "second readKey at B: " . var_export($kB2, true) . "\n";

// 3. Repeated getSize() calls before readKey()
fwrite($pipes[0], "next\n");
fgets($pipes[2]);

$sC1 = $terminal->getSize();
$sC2 = $terminal->getSize();
$sC3 = $terminal->getSize();
echo "repeated getSize at C: {$sC1->cols}x{$sC1->rows}, {$sC2->cols}x{$sC2->rows}, {$sC3->cols}x{$sC3->rows}\n";
$kC1 = $terminal->readKey($zero);
$kC2 = $terminal->readKey($zero);
echo "readKey after repeated getSize at C: " . ($kC1 === Key::Resize ? 'Key::Resize' : var_export($kC1, true)) . "\n";
echo "second readKey at C: " . var_export($kC2, true) . "\n";

// 4. Multiple intervening resizes (C -> D -> E) before readKey(), with getSize() observations
fwrite($pipes[0], "next\n");
fgets($pipes[2]);
$sD = $terminal->getSize();
echo "intervening D: {$sD->cols}x{$sD->rows}\n";

fwrite($pipes[0], "next\n");
fgets($pipes[2]);
$sE = $terminal->getSize();
echo "intervening E: {$sE->cols}x{$sE->rows}\n";

$kE1 = $terminal->readKey($zero);
$kE2 = $terminal->readKey($zero);
echo "readKey after D->E: " . ($kE1 === Key::Resize ? 'Key::Resize' : var_export($kE1, true)) . "\n";
echo "second readKey at E: " . var_export($kE2, true) . "\n";

// 5. Baseline established by readKey() first, then resize to F + queued escape key + getSize() + readKey()
$terminal2 = Terminal::fromStreams($pipes[0], $pipes[1]);
$kInit = $terminal2->readKey($zero);
echo "terminal2 initial readKey at E: " . var_export($kInit, true) . "\n";

fwrite($pipes[0], "next\n");
fgets($pipes[2]);
usleep(2000);
$sF = $terminal2->getSize();
echo "terminal2 getSize at F: {$sF->cols}x{$sF->rows}\n";
$kF1 = $terminal2->readKey($zero);
$kF2 = $terminal2->readKey($zero);
$kF3 = $terminal2->readKey($zero);
echo "terminal2 readKey at F: " . ($kF1 === Key::Resize ? 'Key::Resize' : var_export($kF1, true)) . "\n";
echo "terminal2 queued key after resize at F: " . ($kF2 === Key::Up ? 'Key::Up' : var_export($kF2, true)) . "\n";
echo "terminal2 third readKey at F: " . var_export($kF3, true) . "\n";

$terminal->restoreMode($token);
fwrite($pipes[0], "done\n");
unset($token, $terminal, $terminal2);
foreach ($pipes as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc);
?>
--EXPECT--
baseline A: 100x30
getSize at B: 120x35
readKey after getSize at B: Key::Resize
second readKey at B: NULL
repeated getSize at C: 110x40, 110x40, 110x40
readKey after repeated getSize at C: Key::Resize
second readKey at C: NULL
intervening D: 90x25
intervening E: 130x45
readKey after D->E: Key::Resize
second readKey at E: NULL
terminal2 initial readKey at E: NULL
terminal2 getSize at F: 105x32
terminal2 readKey at F: Key::Resize
terminal2 queued key after resize at F: Key::Up
terminal2 third readKey at F: NULL

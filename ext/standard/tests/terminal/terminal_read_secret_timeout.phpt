--TEST--
Io\Terminal\Terminal: readSecret timeout, empty submission, and cancellation contracts
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
use Io\Terminal\TerminalException;
use Time\Duration;

// 1. Timeout returning null
$proc1 = proc_open(
    [PHP_BINARY, '-r', 'usleep(60000); fgets(STDIN);'],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes1,
);
$terminal1 = Terminal::fromStreams($pipes1[0]);
$start = hrtime(true);
$secret1 = $terminal1->readSecret(Duration::fromMilliseconds(25));
$elapsed_ms = (hrtime(true) - $start) / 1e6;

var_dump($secret1 === null);
var_dump($elapsed_ms >= 20.0);

fwrite($pipes1[0], "done\n");
unset($terminal1);
foreach ($pipes1 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc1);

// 2. Empty submission returning empty string ""
$code2 = '
fwrite(STDERR, "STARTED\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "\n");
fflush(STDOUT);
fwrite(STDERR, "READY\n");
fflush(STDERR);
fgets(STDIN);
';
$proc2 = proc_open(
    [PHP_BINARY, '-r', $code2],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes2,
);
$r0 = fgets($pipes2[2]);
$terminal2 = Terminal::fromStreams($pipes2[0]);
$token2 = $terminal2->enableRawMode();

fwrite($pipes2[0], "START\n");
$r1 = fgets($pipes2[2]);

$secret2 = $terminal2->readSecret(Duration::fromMilliseconds(100));

var_dump($secret2 === "");

fwrite($pipes2[0], "done\n");
unset($terminal2);
foreach ($pipes2 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc2);

// 3. Normal submission returning text
$code3 = '
fwrite(STDERR, "STARTED\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "secret123\n");
fflush(STDOUT);
fwrite(STDERR, "READY\n");
fflush(STDERR);
fgets(STDIN);
';
$proc3 = proc_open(
    [PHP_BINARY, '-r', $code3],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes3,
);
$r0 = fgets($pipes3[2]);
$terminal3 = Terminal::fromStreams($pipes3[0]);
$token3 = $terminal3->enableRawMode();

fwrite($pipes3[0], "START\n");
$r1 = fgets($pipes3[2]);

$secret3 = $terminal3->readSecret(Duration::fromMilliseconds(100));

var_dump($secret3 === "secret123");

fwrite($pipes3[0], "done\n");
unset($terminal3);
foreach ($pipes3 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc3);

// 4. Cancellation (Ctrl+C) throws TerminalException
$code4 = '
fwrite(STDERR, "STARTED\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "\x03");
fflush(STDOUT);
fwrite(STDERR, "READY\n");
fflush(STDERR);
fgets(STDIN);
';
$proc4 = proc_open(
    [PHP_BINARY, '-r', $code4],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes4,
);
$r0 = fgets($pipes4[2]);
$terminal4 = Terminal::fromStreams($pipes4[0]);
$token4 = $terminal4->enableRawMode();

fwrite($pipes4[0], "START\n");
$r1 = fgets($pipes4[2]);

try {
    $terminal4->readSecret(Duration::fromMilliseconds(100));
    echo "FAIL: expected TerminalException on cancellation\n";
} catch (TerminalException $e) {
    echo "Cancellation caught: ", $e->getMessage(), PHP_EOL;
}

fwrite($pipes4[0], "done\n");
unset($terminal4);
foreach ($pipes4 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc4);

// 5. Negative duration throws ValueError
$proc5 = proc_open(
    [PHP_BINARY, '-r', 'fgets(STDIN);'],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes5,
);
$terminal5 = Terminal::fromStreams($pipes5[0]);
try {
    $terminal5->readSecret(Duration::fromSeconds(1)->negate());
    echo "FAIL: expected ValueError on negative duration\n";
} catch (ValueError $e) {
    echo "Negative timeout caught: ", $e->getMessage(), PHP_EOL;
}

fwrite($pipes5[0], "done\n");
unset($terminal5);
foreach ($pipes5 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc5);

// 6. Complete secret plus newline already buffered: zero timeout consumes immediately available bytes without blocking
$code6 = '
fwrite(STDERR, "STARTED\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "zerosecret\n");
fflush(STDOUT);
fwrite(STDERR, "READY\n");
fflush(STDERR);
fgets(STDIN);
';
$proc6 = proc_open(
    [PHP_BINARY, '-r', $code6],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes6,
);
$r0 = fgets($pipes6[2]);
$terminal6 = Terminal::fromStreams($pipes6[0]);
$token6 = $terminal6->enableRawMode();

fwrite($pipes6[0], "START\n");
$r1 = fgets($pipes6[2]);

$secret6 = $terminal6->readSecret(Duration::fromSeconds(0));
var_dump($secret6 === "zerosecret");

fwrite($pipes6[0], "done\n");
unset($terminal6);
foreach ($pipes6 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc6);

// 7. Already-buffered standalone Escape consumed with zero timeout: follows cancellation contract and throws TerminalException
$code7 = '
fwrite(STDERR, "STARTED\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "\x1b");
fflush(STDOUT);
fwrite(STDERR, "READY\n");
fflush(STDERR);
fgets(STDIN);
';
$proc7 = proc_open(
    [PHP_BINARY, '-r', $code7],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes7,
);
$r0 = fgets($pipes7[2]);
$terminal7 = Terminal::fromStreams($pipes7[0]);
$token7 = $terminal7->enableRawMode();

fwrite($pipes7[0], "START\n");
$r1 = fgets($pipes7[2]);

try {
    $terminal7->readSecret(Duration::fromSeconds(0));
    echo "FAIL: expected TerminalException on zero-timeout buffered escape\n";
} catch (TerminalException $e) {
    echo "Zero timeout escape caught: ", $e->getMessage(), PHP_EOL;
}

fwrite($pipes7[0], "done\n");
unset($terminal7);
foreach ($pipes7 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc7);

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
Cancellation caught: Unable to read secret from terminal
Negative timeout caught: Io\Terminal\Terminal::readSecret(): Argument #1 ($timeout) must not be negative
bool(true)
Zero timeout escape caught: Unable to read secret from terminal

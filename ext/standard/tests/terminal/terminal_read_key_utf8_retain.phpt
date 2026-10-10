--TEST--
Io\Terminal\Terminal: readKey retains fragmented UTF-8 across calls on timeout
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

// 1. 3-byte UTF-8 (€: \xe2\x82\xac) split across timeout
$code1 = '
fwrite(STDERR, "STARTED\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "\xe2");
fflush(STDOUT);
fwrite(STDERR, "READY1\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "\x82\xac");
fflush(STDOUT);
fwrite(STDERR, "READY2\n");
fflush(STDERR);
fgets(STDIN);
';
$proc1 = proc_open(
    [PHP_BINARY, '-r', $code1],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes1,
);

$r0 = fgets($pipes1[2]);
$terminal1 = Terminal::fromStreams($pipes1[0]);
$token1 = $terminal1->enableRawMode();

fwrite($pipes1[0], "START\n");
$r1 = fgets($pipes1[2]);

$k1 = $terminal1->readKey(Duration::fromMilliseconds(25));

fwrite($pipes1[0], "GO\n");
$r2 = fgets($pipes1[2]);

$k2 = $terminal1->readKey(Duration::fromMilliseconds(100));

echo "k1 is null: ", var_export($k1 === null, true), PHP_EOL;
echo "k2 is euro: ", var_export($k2 === "€", true), PHP_EOL;
echo "k2 hex: ", bin2hex($k2), PHP_EOL;

fwrite($pipes1[0], "done\n");
unset($terminal1);
foreach ($pipes1 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc1);

// 2. 4-byte UTF-8 emoji (😀: \xf0\x9f\x98\x80) split across timeout
$code2 = '
fwrite(STDERR, "STARTED\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "\xf0\x9f");
fflush(STDOUT);
fwrite(STDERR, "READY1\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "\x98\x80");
fflush(STDOUT);
fwrite(STDERR, "READY2\n");
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

$k3 = $terminal2->readKey(Duration::fromMilliseconds(25));

fwrite($pipes2[0], "GO\n");
$r2 = fgets($pipes2[2]);

$k4 = $terminal2->readKey(Duration::fromMilliseconds(100));

echo "k3 is null: ", var_export($k3 === null, true), PHP_EOL;
echo "k4 is emoji: ", var_export($k4 === "😀", true), PHP_EOL;
echo "k4 hex: ", bin2hex($k4), PHP_EOL;

fwrite($pipes2[0], "done\n");
unset($terminal2);
foreach ($pipes2 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc2);

// 3. 4-byte UTF-8 sequence where more than one byte (2 bytes: \xf0\x9f) is already pending,
// and the remaining bytes arrive later than sequenceTimeout (20ms) but within the next call's
// overall timeout (1000ms).
$code3 = '
fwrite(STDERR, "STARTED\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "\xf0\x9f");
fflush(STDOUT);
fwrite(STDERR, "READY1\n");
fflush(STDERR);
fgets(STDIN);

usleep(40000);
fwrite(STDOUT, "\x98\x80");
fflush(STDOUT);
fwrite(STDERR, "READY2\n");
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

$k5 = $terminal3->readKey(Duration::fromMilliseconds(30), Duration::fromMilliseconds(20));

fwrite($pipes3[0], "GO\n");

$k6 = $terminal3->readKey(Duration::fromMilliseconds(1000), Duration::fromMilliseconds(20));

echo "k5 is null: ", var_export($k5 === null, true), PHP_EOL;
echo "k6 is emoji: ", var_export($k6 === "😀", true), PHP_EOL;
echo "k6 hex: ", bin2hex($k6), PHP_EOL;

$r2 = fgets($pipes3[2]);
fwrite($pipes3[0], "done\n");
unset($terminal3);
foreach ($pipes3 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc3);

?>
--EXPECT--
k1 is null: true
k2 is euro: true
k2 hex: e282ac
k3 is null: true
k4 is emoji: true
k4 hex: f09f9880
k5 is null: true
k6 is emoji: true
k6 hex: f09f9880

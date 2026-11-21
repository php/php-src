--TEST--
Io\Terminal\Terminal: readKey partial escape sequence and overall deadline timing
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

// Case 1: Overall deadline 30ms, ESC and [ sent before 30ms, A sent after release.
// The first call returns incomplete escape sequence "\x1b[" (hex 1b5b).
// The subsequent call returns "A".
$code1 = '
fwrite(STDERR, "STARTED\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "\x1b[");
fflush(STDOUT);
fwrite(STDERR, "READY1\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "A");
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

$k1 = $terminal1->readKey(Duration::fromMilliseconds(30), Duration::fromMilliseconds(100));

fwrite($pipes1[0], "GO\n");
$r2 = fgets($pipes1[2]);

$k2 = $terminal1->readKey(Duration::fromMilliseconds(100));

echo "Case 1 k1 is string: ", var_export(is_string($k1), true), PHP_EOL;
echo "Case 1 k1 hex: ", bin2hex($k1), PHP_EOL;
echo "Case 1 k2: ", var_export($k2, true), PHP_EOL;

fwrite($pipes1[0], "done\n");
unset($terminal1);
foreach ($pipes1 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc1);

// Case 2: Overall deadline 30ms, ESC sent at 0ms, [ arrives after release.
// Because [ does not arrive before the 30ms deadline, the first call returns Key::Escape.
// The subsequent call reads [ independently.
$code2 = '
fwrite(STDERR, "STARTED\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "\x1b");
fflush(STDOUT);
fwrite(STDERR, "READY1\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "[");
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

$k3 = $terminal2->readKey(Duration::fromMilliseconds(30), Duration::fromMilliseconds(100));

fwrite($pipes2[0], "GO\n");
$r2 = fgets($pipes2[2]);

$k4 = $terminal2->readKey(Duration::fromMilliseconds(100));

echo "Case 2 k3 is Escape: ", var_export($k3 === Key::Escape, true), PHP_EOL;
echo "Case 2 k4: ", var_export($k4, true), PHP_EOL;

fwrite($pipes2[0], "done\n");
unset($terminal2);
foreach ($pipes2 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc2);

?>
--EXPECT--
Case 1 k1 is string: true
Case 1 k1 hex: 1b5b
Case 1 k2: 'A'
Case 2 k3 is Escape: true
Case 2 k4: '['

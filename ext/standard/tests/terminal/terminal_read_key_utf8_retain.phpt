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
$code1 = 'fwrite(STDOUT, "\xe2"); fflush(STDOUT); usleep(60000); fwrite(STDOUT, "\x82\xac"); fflush(STDOUT); fgets(STDIN);';
$proc1 = proc_open(
    [PHP_BINARY, '-r', $code1],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes1,
);

$terminal1 = Terminal::fromStreams($pipes1[0]);
$terminal1->enableRawMode();

$k1 = $terminal1->readKey(Duration::fromMilliseconds(25));
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
$code2 = 'fwrite(STDOUT, "\xf0\x9f"); fflush(STDOUT); usleep(60000); fwrite(STDOUT, "\x98\x80"); fflush(STDOUT); fgets(STDIN);';
$proc2 = proc_open(
    [PHP_BINARY, '-r', $code2],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes2,
);

$terminal2 = Terminal::fromStreams($pipes2[0]);
$terminal2->enableRawMode();

$k3 = $terminal2->readKey(Duration::fromMilliseconds(25));
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

?>
--EXPECT--
k1 is null: true
k2 is euro: true
k2 hex: e282ac
k3 is null: true
k4 is emoji: true
k4 hex: f09f9880

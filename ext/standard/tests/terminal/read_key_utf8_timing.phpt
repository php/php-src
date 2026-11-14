--TEST--
Io\Terminal\Terminal: readKey UTF-8 timing and overall timeout behavior
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

function spawn_pty_child(string $childCode): array {
    $proc = proc_open(
        [PHP_BINARY, '-r', $childCode],
        [
            0 => ['pty'],
            1 => ['pty'],
            2 => ['pipe', 'w'],
        ],
        $pipes,
    );
    $started = fgets($pipes[2]);
    $terminal = Terminal::fromStreams($pipes[0]);
    $token = $terminal->enableRawMode();
    return [$proc, $pipes, $terminal, $token];
}

// Case 1: Complete queued 3-byte UTF-8 (€) with Duration::fromSeconds(0)
$code1 = '
fwrite(STDERR, "STARTED\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "\xe2\x82\xac");
fflush(STDOUT);
fwrite(STDERR, "READY\n");
fflush(STDERR);
fgets(STDIN);
';
[$proc1, $pipes1, $terminal1, $token1] = spawn_pty_child($code1);
fwrite($pipes1[0], "START\n");
$r1 = fgets($pipes1[2]);

$k1 = $terminal1->readKey(Duration::fromSeconds(0));
echo "Case 1: ", var_export($k1 === "€", true), " hex: ", bin2hex($k1), PHP_EOL;

fwrite($pipes1[0], "done\n");
$terminal1->restoreMode($token1);
unset($terminal1);
foreach ($pipes1 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc1);

// Case 2: Complete queued 4-byte UTF-8 (😀) with Duration::fromSeconds(0)
$code2 = '
fwrite(STDERR, "STARTED\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "\xf0\x9f\x98\x80");
fflush(STDOUT);
fwrite(STDERR, "READY\n");
fflush(STDERR);
fgets(STDIN);
';
[$proc2, $pipes2, $terminal2, $token2] = spawn_pty_child($code2);
fwrite($pipes2[0], "START\n");
$r2 = fgets($pipes2[2]);

$k2 = $terminal2->readKey(Duration::fromSeconds(0));
echo "Case 2: ", var_export($k2 === "😀", true), " hex: ", bin2hex($k2), PHP_EOL;

fwrite($pipes2[0], "done\n");
$terminal2->restoreMode($token2);
unset($terminal2);
foreach ($pipes2 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc2);

// Case 3: Retained UTF-8 lead byte followed by queued continuation bytes completed by subsequent Duration::fromSeconds(0)
$code3 = '
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
[$proc3, $pipes3, $terminal3, $token3] = spawn_pty_child($code3);
fwrite($pipes3[0], "START\n");
$r3_1 = fgets($pipes3[2]);

$k3_1 = $terminal3->readKey(Duration::fromMilliseconds(25));
echo "Case 3 k3_1 is null: ", var_export($k3_1 === null, true), PHP_EOL;

fwrite($pipes3[0], "GO\n");
$r3_2 = fgets($pipes3[2]);

$k3_2 = $terminal3->readKey(Duration::fromSeconds(0));
echo "Case 3 k3_2 is euro: ", var_export($k3_2 === "€", true), " hex: ", bin2hex($k3_2), PHP_EOL;

fwrite($pipes3[0], "done\n");
$terminal3->restoreMode($token3);
unset($terminal3);
foreach ($pipes3 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc3);

// Case 4: Fresh fragmented UTF-8 where continuation arrives later than sequenceTimeout (50ms vs 20ms) but before finite overall timeout (500ms)
$code4 = '
fwrite(STDERR, "STARTED\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "\xe2");
fflush(STDOUT);
fwrite(STDERR, "READY1\n");
fflush(STDERR);
fgets(STDIN);

usleep(50000);
fwrite(STDOUT, "\x82\xac");
fflush(STDOUT);
fwrite(STDERR, "READY2\n");
fflush(STDERR);
fgets(STDIN);
';
[$proc4, $pipes4, $terminal4, $token4] = spawn_pty_child($code4);
fwrite($pipes4[0], "START\n");
$r4_1 = fgets($pipes4[2]);

fwrite($pipes4[0], "GO\n");
$k4 = $terminal4->readKey(Duration::fromMilliseconds(500), Duration::fromMilliseconds(20));
echo "Case 4: ", var_export($k4 === "€", true), " hex: ", bin2hex($k4), PHP_EOL;

$r4_2 = fgets($pipes4[2]);
fwrite($pipes4[0], "done\n");
$terminal4->restoreMode($token4);
unset($terminal4);
foreach ($pipes4 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc4);

// Case 5: readKey(timeout: null, sequenceTimeout: 25ms) where continuation arrives after >25ms (60ms); blocks and returns the complete character
$code5 = '
fwrite(STDERR, "STARTED\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "\xe2");
fflush(STDOUT);
fwrite(STDERR, "READY1\n");
fflush(STDERR);
fgets(STDIN);

usleep(60000);
fwrite(STDOUT, "\x82\xac");
fflush(STDOUT);
fwrite(STDERR, "READY2\n");
fflush(STDERR);
fgets(STDIN);
';
[$proc5, $pipes5, $terminal5, $token5] = spawn_pty_child($code5);
fwrite($pipes5[0], "START\n");
$r5_1 = fgets($pipes5[2]);

fwrite($pipes5[0], "GO\n");
$k5 = $terminal5->readKey(timeout: null, sequenceTimeout: Duration::fromMilliseconds(25));
echo "Case 5: ", var_export($k5 === "€", true), " hex: ", bin2hex($k5), PHP_EOL;

$r5_2 = fgets($pipes5[2]);
fwrite($pipes5[0], "done\n");
$terminal5->restoreMode($token5);
unset($terminal5);
foreach ($pipes5 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc5);

?>
--EXPECT--
Case 1: true hex: e282ac
Case 2: true hex: f09f9880
Case 3 k3_1 is null: true
Case 3 k3_2 is euro: true hex: e282ac
Case 4: true hex: e282ac
Case 5: true hex: e282ac

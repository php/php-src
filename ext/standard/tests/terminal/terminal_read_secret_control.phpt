--TEST--
Io\Terminal\Terminal: readSecret cancels and throws on malformed UTF-8 with control characters, fragmented UTF-8, and escape aborts
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

// 1. Send malformed UTF-8 sequence followed immediately by Ctrl+C (\x03) and Enter (\r)
$code1 = 'fwrite(STDOUT, "\xc2\x03\r"); fflush(STDOUT); fgets(STDIN);';
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

try {
    $secret = $terminal1->readSecret();
    echo "FAIL: secret was read: ", bin2hex($secret), "\n";
} catch (TerminalException $e) {
    echo "SUCCESS: caught ", $e::class, ": ", $e->getMessage(), "\n";
}

fwrite($pipes1[0], "done\n");
unset($terminal1);
foreach ($pipes1 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc1);

// 2. Fragmented UTF-8: \xC3 followed 60ms later by \xA9\n must not drop \xC3
$code2 = 'fwrite(STDOUT, "\xc3"); fflush(STDOUT); usleep(60000); fwrite(STDOUT, "\xa9\n"); fflush(STDOUT); fgets(STDIN);';
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
$secret2 = $terminal2->readSecret();
echo "Fragmented UTF-8: ", bin2hex($secret2), "\n";

fwrite($pipes2[0], "done\n");
unset($terminal2);
foreach ($pipes2 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc2);

// 3. Escape followed immediately by PTY disconnect
$code3 = 'fwrite(STDOUT, "\x1b"); fflush(STDOUT); usleep(10000); exit(0);';
$proc3 = proc_open(
    [PHP_BINARY, '-r', $code3],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes3,
);

$terminal3 = Terminal::fromStreams($pipes3[0]);
try {
    $terminal3->readSecret();
    echo "FAIL: expected exception on disconnect\n";
} catch (TerminalException $e) {
    echo "Disconnect: caught ", $e::class, ": ", $e->getMessage(), "\n";
}

unset($terminal3);
foreach ($pipes3 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc3);

// 4. Escape followed immediately by Ctrl+C (\x03)
$code4 = 'fwrite(STDOUT, "\x1b\x03"); fflush(STDOUT); fgets(STDIN);';
$proc4 = proc_open(
    [PHP_BINARY, '-r', $code4],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes4,
);

$terminal4 = Terminal::fromStreams($pipes4[0]);
try {
    $terminal4->readSecret();
    echo "FAIL: expected abort on escape+Ctrl+C\n";
} catch (TerminalException $e) {
    echo "Escape+Ctrl+C: caught ", $e::class, ": ", $e->getMessage(), "\n";
}

fwrite($pipes4[0], "done\n");
unset($terminal4);
foreach ($pipes4 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc4);

// 5. Escape followed immediately by Enter (\r)
$code5 = 'fwrite(STDOUT, "\x1b\r"); fflush(STDOUT); fgets(STDIN);';
$proc5 = proc_open(
    [PHP_BINARY, '-r', $code5],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes5,
);

$terminal5 = Terminal::fromStreams($pipes5[0]);
try {
    $terminal5->readSecret();
    echo "FAIL: expected abort on escape+Enter\n";
} catch (TerminalException $e) {
    echo "Escape+Enter: caught ", $e::class, ": ", $e->getMessage(), "\n";
}

fwrite($pipes5[0], "done\n");
unset($terminal5);
foreach ($pipes5 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc5);

?>
--EXPECT--
SUCCESS: caught Io\Terminal\TerminalException: Unable to read secret from terminal
Fragmented UTF-8: c3a9
Disconnect: caught Io\Terminal\TerminalException: Unable to read secret from terminal
Escape+Ctrl+C: caught Io\Terminal\TerminalException: Unable to read secret from terminal
Escape+Enter: caught Io\Terminal\TerminalException: Unable to read secret from terminal

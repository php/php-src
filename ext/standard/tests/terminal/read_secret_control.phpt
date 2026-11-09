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

// 2. Escape followed by Ctrl+C (\x1b\x03) cancels secret input immediately
$code2 = 'fwrite(STDOUT, "\x1b\x03\r"); fflush(STDOUT); fgets(STDIN);';
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

try {
    $secret = $terminal2->readSecret();
    echo "FAIL: secret was read: ", bin2hex($secret), "\n";
} catch (TerminalException $e) {
    echo "SUCCESS: caught ", $e::class, ": ", $e->getMessage(), "\n";
}

fwrite($pipes2[0], "done\n");
unset($terminal2);
foreach ($pipes2 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc2);

// 3. CSI sequence containing Ctrl+C aborts
$code3 = 'fwrite(STDOUT, "\x1b[\x03A\r"); fflush(STDOUT); fgets(STDIN);';
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
    $secret = $terminal3->readSecret();
    echo "FAIL: secret was read: ", bin2hex($secret), "\n";
} catch (TerminalException $e) {
    echo "SUCCESS: caught ", $e::class, ": ", $e->getMessage(), "\n";
}

fwrite($pipes3[0], "done\n");
unset($terminal3);
foreach ($pipes3 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc3);

// 4. SS3 sequence containing Ctrl+C aborts
$code4 = 'fwrite(STDOUT, "\x1bO\x03\r"); fflush(STDOUT); fgets(STDIN);';
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
    $secret = $terminal4->readSecret();
    echo "FAIL: secret was read: ", bin2hex($secret), "\n";
} catch (TerminalException $e) {
    echo "SUCCESS: caught ", $e::class, ": ", $e->getMessage(), "\n";
}

fwrite($pipes4[0], "done\n");
unset($terminal4);
foreach ($pipes4 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc4);

// 5. Escape followed by enter (\x1b\r) aborts
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
    $secret = $terminal5->readSecret();
    echo "FAIL: secret was read: ", bin2hex($secret), "\n";
} catch (TerminalException $e) {
    echo "SUCCESS: caught ", $e::class, ": ", $e->getMessage(), "\n";
}

fwrite($pipes5[0], "done\n");
unset($terminal5);
foreach ($pipes5 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc5);

// 6. Escape followed by escape (\x1b\x1b) aborts
$code6 = 'fwrite(STDOUT, "\x1b\x1b\r"); fflush(STDOUT); fgets(STDIN);';
$proc6 = proc_open(
    [PHP_BINARY, '-r', $code6],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes6,
);

$terminal6 = Terminal::fromStreams($pipes6[0]);

try {
    $secret = $terminal6->readSecret();
    echo "FAIL: secret was read: ", bin2hex($secret), "\n";
} catch (TerminalException $e) {
    echo "SUCCESS: caught ", $e::class, ": ", $e->getMessage(), "\n";
}

fwrite($pipes6[0], "done\n");
unset($terminal6);
foreach ($pipes6 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc6);

// 7. Escape followed by Ctrl+D (\x1b\x04) aborts
$code7 = 'fwrite(STDOUT, "\x1b\x04\r"); fflush(STDOUT); fgets(STDIN);';
$proc7 = proc_open(
    [PHP_BINARY, '-r', $code7],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes7,
);

$terminal7 = Terminal::fromStreams($pipes7[0]);

try {
    $secret = $terminal7->readSecret();
    echo "FAIL: secret was read: ", bin2hex($secret), "\n";
} catch (TerminalException $e) {
    echo "SUCCESS: caught ", $e::class, ": ", $e->getMessage(), "\n";
}

fwrite($pipes7[0], "done\n");
unset($terminal7);
foreach ($pipes7 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc7);

// 8. CSI sequence with cancellation byte \x04 aborts
$code8 = 'fwrite(STDOUT, "\x1b[\x04\r"); fflush(STDOUT); fgets(STDIN);';
$proc8 = proc_open(
    [PHP_BINARY, '-r', $code8],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes8,
);

$terminal8 = Terminal::fromStreams($pipes8[0]);

try {
    $secret = $terminal8->readSecret();
    echo "FAIL: secret was read: ", bin2hex($secret), "\n";
} catch (TerminalException $e) {
    echo "SUCCESS: caught ", $e::class, ": ", $e->getMessage(), "\n";
}

fwrite($pipes8[0], "done\n");
unset($terminal8);
foreach ($pipes8 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc8);

// 9. CSI sequence with cancellation byte \x1b aborts
$code9 = 'fwrite(STDOUT, "\x1b[\x1b\r"); fflush(STDOUT); fgets(STDIN);';
$proc9 = proc_open(
    [PHP_BINARY, '-r', $code9],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes9,
);

$terminal9 = Terminal::fromStreams($pipes9[0]);

try {
    $secret = $terminal9->readSecret();
    echo "FAIL: secret was read: ", bin2hex($secret), "\n";
} catch (TerminalException $e) {
    echo "SUCCESS: caught ", $e::class, ": ", $e->getMessage(), "\n";
}

fwrite($pipes9[0], "done\n");
unset($terminal9);
foreach ($pipes9 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc9);

// 10. CSI sequence with cancellation byte \r aborts
$code10 = 'fwrite(STDOUT, "\x1b[\r"); fflush(STDOUT); fgets(STDIN);';
$proc10 = proc_open(
    [PHP_BINARY, '-r', $code10],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes10,
);

$terminal10 = Terminal::fromStreams($pipes10[0]);

try {
    $secret = $terminal10->readSecret();
    echo "FAIL: secret was read: ", bin2hex($secret), "\n";
} catch (TerminalException $e) {
    echo "SUCCESS: caught ", $e::class, ": ", $e->getMessage(), "\n";
}

fwrite($pipes10[0], "done\n");
unset($terminal10);
foreach ($pipes10 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc10);

// 11. SS3 sequence with cancellation byte \x04 aborts
$code11 = 'fwrite(STDOUT, "\x1bO\x04\r"); fflush(STDOUT); fgets(STDIN);';
$proc11 = proc_open(
    [PHP_BINARY, '-r', $code11],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes11,
);

$terminal11 = Terminal::fromStreams($pipes11[0]);

try {
    $secret = $terminal11->readSecret();
    echo "FAIL: secret was read: ", bin2hex($secret), "\n";
} catch (TerminalException $e) {
    echo "SUCCESS: caught ", $e::class, ": ", $e->getMessage(), "\n";
}

fwrite($pipes11[0], "done\n");
unset($terminal11);
foreach ($pipes11 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc11);

?>
--EXPECT--
SUCCESS: caught Io\Terminal\TerminalException: Unable to read secret from terminal
SUCCESS: caught Io\Terminal\TerminalException: Unable to read secret from terminal
SUCCESS: caught Io\Terminal\TerminalException: Unable to read secret from terminal
SUCCESS: caught Io\Terminal\TerminalException: Unable to read secret from terminal
SUCCESS: caught Io\Terminal\TerminalException: Unable to read secret from terminal
SUCCESS: caught Io\Terminal\TerminalException: Unable to read secret from terminal
SUCCESS: caught Io\Terminal\TerminalException: Unable to read secret from terminal
SUCCESS: caught Io\Terminal\TerminalException: Unable to read secret from terminal
SUCCESS: caught Io\Terminal\TerminalException: Unable to read secret from terminal
SUCCESS: caught Io\Terminal\TerminalException: Unable to read secret from terminal
SUCCESS: caught Io\Terminal\TerminalException: Unable to read secret from terminal

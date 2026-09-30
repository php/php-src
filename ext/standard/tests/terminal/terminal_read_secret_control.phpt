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

use Io\Terminal\SystemTerminal;
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

$terminal1 = SystemTerminal::fromStreams($pipes1[0]);

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

$terminal2 = SystemTerminal::fromStreams($pipes2[0]);
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

$terminal3 = SystemTerminal::fromStreams($pipes3[0]);
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

$terminal4 = SystemTerminal::fromStreams($pipes4[0]);
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

$terminal5 = SystemTerminal::fromStreams($pipes5[0]);
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

// 6. CSI followed by Ctrl+C (\x1b[\x03) cancels
$code6 = 'fwrite(STDOUT, "\x1b[\x03"); fflush(STDOUT); fgets(STDIN);';
$proc6 = proc_open(
    [PHP_BINARY, '-r', $code6],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes6,
);
$terminal6 = SystemTerminal::fromStreams($pipes6[0]);
try {
    $terminal6->readSecret();
    echo "FAIL: expected abort on CSI+Ctrl+C\n";
} catch (TerminalException $e) {
    echo "CSI+Ctrl+C: caught ", $e::class, ": ", $e->getMessage(), "\n";
}
fwrite($pipes6[0], "done\n");
unset($terminal6);
foreach ($pipes6 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc6);

// 7. CSI followed by Ctrl+D (\x1b[\x04) cancels
$code7 = 'fwrite(STDOUT, "\x1b[\x04"); fflush(STDOUT); fgets(STDIN);';
$proc7 = proc_open(
    [PHP_BINARY, '-r', $code7],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes7,
);
$terminal7 = SystemTerminal::fromStreams($pipes7[0]);
try {
    $terminal7->readSecret();
    echo "FAIL: expected abort on CSI+Ctrl+D\n";
} catch (TerminalException $e) {
    echo "CSI+Ctrl+D: caught ", $e::class, ": ", $e->getMessage(), "\n";
}
fwrite($pipes7[0], "done\n");
unset($terminal7);
foreach ($pipes7 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc7);

// 8. SS3 followed by Ctrl+C (\x1bO\x03) cancels
$code8 = 'fwrite(STDOUT, "\x1bO\x03"); fflush(STDOUT); fgets(STDIN);';
$proc8 = proc_open(
    [PHP_BINARY, '-r', $code8],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes8,
);
$terminal8 = SystemTerminal::fromStreams($pipes8[0]);
try {
    $terminal8->readSecret();
    echo "FAIL: expected abort on SS3+Ctrl+C\n";
} catch (TerminalException $e) {
    echo "SS3+Ctrl+C: caught ", $e::class, ": ", $e->getMessage(), "\n";
}
fwrite($pipes8[0], "done\n");
unset($terminal8);
foreach ($pipes8 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc8);

// 9. Valid CSI sequence (e.g. Up arrow \x1b[A) ignored and secret input continues
$code9 = 'fwrite(STDOUT, "\x1b[Asecret\n"); fflush(STDOUT); fgets(STDIN);';
$proc9 = proc_open(
    [PHP_BINARY, '-r', $code9],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes9,
);
$terminal9 = SystemTerminal::fromStreams($pipes9[0]);
$secret9 = $terminal9->readSecret();
echo "Valid CSI Up ignored: ", bin2hex($secret9), "\n";
fwrite($pipes9[0], "done\n");
unset($terminal9);
foreach ($pipes9 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc9);

// 10. Valid SS3 sequence (e.g. F1 \x1bOP) ignored and secret input continues
$code10 = 'fwrite(STDOUT, "\x1bOPpassword\n"); fflush(STDOUT); fgets(STDIN);';
$proc10 = proc_open(
    [PHP_BINARY, '-r', $code10],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes10,
);
$terminal10 = SystemTerminal::fromStreams($pipes10[0]);
$secret10 = $terminal10->readSecret();
echo "Valid SS3 F1 ignored: ", bin2hex($secret10), "\n";
fwrite($pipes10[0], "done\n");
unset($terminal10);
foreach ($pipes10 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc10);

// 11. Internal 25ms timeout inside incomplete CSI ends sequence skipping and continues secret
$code11 = 'fwrite(STDOUT, "\x1b["); fflush(STDOUT); usleep(35000); fwrite(STDOUT, "mysecret\n"); fflush(STDOUT); fgets(STDIN);';
$proc11 = proc_open(
    [PHP_BINARY, '-r', $code11],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes11,
);
$terminal11 = SystemTerminal::fromStreams($pipes11[0]);
$secret11 = $terminal11->readSecret();
echo "Internal timeout continue: ", bin2hex($secret11), "\n";
fwrite($pipes11[0], "done\n");
unset($terminal11);
foreach ($pipes11 as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc11);

?>
--EXPECT--
SUCCESS: caught Io\Terminal\TerminalException: Unable to read secret from terminal
Fragmented UTF-8: c3a9
Disconnect: caught Io\Terminal\TerminalException: Unable to read secret from terminal
Escape+Ctrl+C: caught Io\Terminal\TerminalException: Unable to read secret from terminal
Escape+Enter: caught Io\Terminal\TerminalException: Unable to read secret from terminal
CSI+Ctrl+C: caught Io\Terminal\TerminalException: Unable to read secret from terminal
CSI+Ctrl+D: caught Io\Terminal\TerminalException: Unable to read secret from terminal
SS3+Ctrl+C: caught Io\Terminal\TerminalException: Unable to read secret from terminal
Valid CSI Up ignored: 736563726574
Valid SS3 F1 ignored: 70617373776f7264
Internal timeout continue: 6d79736563726574

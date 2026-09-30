--TEST--
Io\Terminal\SystemTerminal: readLine raw-mode lease conflict, cross-instance validation, and PTY line contracts
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
use Time\Duration;

$code = '
fwrite(STDERR, "STARTED\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "canonical line\n");
fflush(STDOUT);
fwrite(STDERR, "READY1\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "second line\n");
fflush(STDOUT);
fwrite(STDERR, "READY2\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "\xc3");
fflush(STDOUT);
fwrite(STDERR, "READY3\n");
fflush(STDERR);
fgets(STDIN);

fwrite(STDOUT, "\xa9clair\n");
fflush(STDOUT);
fwrite(STDERR, "READY4\n");
fflush(STDERR);
fgets(STDIN);
';

$proc = proc_open(
    [PHP_BINARY, '-r', $code],
    [
        0 => ['pipe', 'r'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes,
);

fgets($pipes[2]); // Wait for STARTED

$t1 = SystemTerminal::fromStreams($pipes[1]);
$t2 = SystemTerminal::fromStreams($pipes[1]);

// 1. Same-instance active raw lease throws TerminalException
$token = $t1->enableRawMode();
try {
    $t1->readLine();
    echo "FAIL: t1->readLine() succeeded while raw mode is active\n";
} catch (TerminalException $e) {
    echo "Same-instance: ", $e->getMessage(), PHP_EOL;
}

// 2. Cross-instance active raw lease on same terminal device throws TerminalException
try {
    $t2->readLine();
    echo "FAIL: t2->readLine() succeeded while raw mode is active on t1\n";
} catch (TerminalException $e) {
    echo "Cross-instance: ", $e->getMessage(), PHP_EOL;
}

// 3. Cross-instance restoreMode($token) succeeds and restores mode across wrappers
$restored = $t2->restoreMode($token);
var_dump($restored);

// 4. readLine on original wrapper works normally after cross-wrapper restoration
fwrite($pipes[0], "GO\n");
fgets($pipes[2]); // Wait for READY1
var_dump($t1->readLine());

// 5. Subsequent readLine calls continue to work normally
fwrite($pipes[0], "GO\n");
fgets($pipes[2]); // Wait for READY2
var_dump($t2->readLine());

// 6. Fragmented UTF-8 prepending: pending UTF-8 bytes from readKey are prepended to readLine
$token2 = $t1->enableRawMode();
fwrite($pipes[0], "GO\n");
fgets($pipes[2]); // Wait for READY3
$key = $t1->readKey(Duration::fromMilliseconds(25));
var_dump($key === null); // Incomplete, retained in pending_utf8

// Restore raw mode so readLine can proceed
$t1->restoreMode($token2);

// Provide continuation byte \xa9 followed by remainder of line
fwrite($pipes[0], "GO\n");
fgets($pipes[2]); // Wait for READY4
$line = $t1->readLine();
var_dump($line);

// 7. Immediate EOF on PTY returns null
fwrite($pipes[0], "DONE\n");
usleep(50000); // Allow child process to finish
var_dump($t1->readLine());

unset($t1, $t2);
foreach ($pipes as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc);

?>
--EXPECT--
Same-instance: Cannot read a line while raw mode is active for this terminal
Cross-instance: Cannot read a line while raw mode is active for this terminal
bool(true)
string(14) "canonical line"
string(11) "second line"
bool(true)
string(7) "éclair"
NULL

--TEST--
Io\Terminal\Terminal: readSecret cancels and throws on malformed UTF-8 with control characters
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

// Send malformed UTF-8 sequence followed immediately by Ctrl+C (\x03) and Enter (\r)
$code = 'fwrite(STDOUT, "\xc2\x03\r");';
$proc = proc_open(
    [PHP_BINARY, '-r', $code],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes,
);

$terminal = Terminal::fromStreams($pipes[0]);

try {
    $secret = $terminal->readSecret();
    echo "FAIL: secret was read: ", bin2hex($secret), "\n";
} catch (TerminalException $e) {
    echo "SUCCESS: caught ", $e::class, ": ", $e->getMessage(), "\n";
}

unset($terminal);
foreach ($pipes as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc);
?>
--EXPECT--
SUCCESS: caught Io\Terminal\TerminalException: Unable to read secret from terminal

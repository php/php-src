--TEST--
Io\Terminal\Terminal: readKey detects resize and updates terminal size
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
use Io\Terminal\SystemTerminal;
use Time\Duration;

$code = '
exec("stty rows 30 cols 100");
fwrite(STDERR, "READY1\n");
fflush(STDERR);
fgets(STDIN);
exec("stty rows 35 cols 120");
fwrite(STDERR, "READY2\n");
fflush(STDERR);
fgets(STDIN);
';

$proc = proc_open(
    [PHP_BINARY, '-r', $code],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes,
);

$r1 = fgets($pipes[2]);
$terminal = SystemTerminal::fromStreams($pipes[0]);
$terminal->enableRawMode();

$s1 = $terminal->getSize();
echo "s1: {$s1->cols}x{$s1->rows}\n";

// Signal child to change window size
fwrite($pipes[0], "next\n");
$r2 = fgets($pipes[2]);

// readKey should detect size change and return Key::Resize
$key = $terminal->readKey(Duration::fromSeconds(0));
var_dump($key === Key::Resize);

$s2 = $terminal->getSize();
echo "s2: {$s2->cols}x{$s2->rows}\n";

fwrite($pipes[0], "done\n");
unset($terminal);
foreach ($pipes as $pipe) {
    if (is_resource($pipe)) fclose($pipe);
}
proc_close($proc);
?>
--EXPECT--
s1: 100x30
bool(true)
s2: 120x35

--TEST--
Io\Terminal\SystemTerminal: readKey preserves unsupported CSI sequences
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
    if (is_resource($pipe)) {
        fclose($pipe);
    }
}
proc_close($proc);
?>
--FILE--
<?php

use Io\Terminal\Key;
use Io\Terminal\SystemTerminal;
use Time\Duration;

$payload = "\x1b[1;5A\x1b[2~\x1bOP\x1b[A";
$code = 'fwrite(STDOUT, ' . var_export($payload, true) . ');';

$proc = proc_open(
    [PHP_BINARY, '-r', $code],
    [
        0 => ['pty'],
        1 => ['pty'],
        2 => ['pipe', 'w'],
    ],
    $pipes,
);

$terminal = SystemTerminal::fromStreams($pipes[0]);
$terminal->enableRawMode();
$timeout = Duration::fromSeconds(1);

foreach (range(1, 4) as $_) {
    $key = $terminal->readKey($timeout);

    if ($key instanceof Key) {
        echo $key->name, "\n";
    } elseif (is_string($key)) {
        echo bin2hex($key), "\n";
    } else {
        echo "false\n";
    }
}

unset($terminal);
foreach ($pipes as $pipe) {
    if (is_resource($pipe)) {
        fclose($pipe);
    }
}
proc_close($proc);
?>
--EXPECT--
1b5b313b3541
1b5b327e
F1
Up

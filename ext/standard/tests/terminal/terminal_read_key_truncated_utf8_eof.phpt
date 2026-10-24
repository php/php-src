--TEST--
Io\Terminal\Terminal: readKey throws on truncated UTF-8 at EOF and preserves timeouts on open writer
--SKIPIF--
<?php
if (!class_exists(\Io\Terminal\Terminal::class)) die('skip Io\Terminal\Terminal not available');
if (!class_exists(\Time\Duration::class)) die('skip Time\Duration not available');
if (!function_exists('proc_open')) die('skip proc_open is not available');
?>
--FILE--
<?php

use Io\Terminal\Terminal;
use Io\Terminal\TerminalException;
use Time\Duration;

function assertThrowsEof(callable $fn, string $label): void {
    try {
        $fn();
        echo "FAIL ($label): did not throw\n";
    } catch (TerminalException $e) {
        if ($e->getMessage() === "End of file reached on terminal input stream") {
            echo "PASS ($label)\n";
        } else {
            echo "FAIL ($label): unexpected message: " . $e->getMessage() . "\n";
        }
    }
}

// 1. Closed sockets: truncated 2-, 3-, and 4-byte prefixes with default, zero, and positive timeouts
$prefixes = [
    '2-byte \xC3' => "\xC3",
    '3-byte \xE2' => "\xE2",
    '3-byte \xE2\x82' => "\xE2\x82",
    '4-byte \xF0' => "\xF0",
    '4-byte \xF0\x9F\x92' => "\xF0\x9F\x92",
];

$timeouts = [
    'default' => null,
    'zero' => Duration::fromSeconds(0),
    'positive' => Duration::fromMilliseconds(30),
];

foreach ($prefixes as $pname => $prefix) {
    foreach ($timeouts as $tname => $timeout) {
        [$input, $writer] = stream_socket_pair(
            PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
            STREAM_SOCK_STREAM,
            PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
        );
        fwrite($writer, $prefix);
        fclose($writer);

        $term = Terminal::fromStreams($input);
        for ($i = 1; $i <= 2; $i++) {
            assertThrowsEof(function() use ($term, $timeout) {
                if ($timeout === null) {
                    $term->readKey();
                } else {
                    $term->readKey($timeout);
                }
            }, "closed socket $pname $tname call $i");
        }
        fclose($input);
    }
}

// 2. Anonymous subprocess pipe whose writer exits after emitting incomplete bytes
$proc = proc_open(
    [PHP_BINARY, '-r', 'fwrite(STDOUT, "\xE2"); fflush(STDOUT); exit(0);'],
    [1 => ['pipe', 'w']],
    $pipes
);

if (!is_resource($proc) || !isset($pipes[1])) {
    echo "FAIL (pipe after exit): proc_open failed to spawn child\n";
    if (is_resource($proc)) {
        proc_close($proc);
    }
    if (isset($pipes[1]) && is_resource($pipes[1])) {
        fclose($pipes[1]);
    }
} else {
    $timeoutNs = 5_000_000_000; // 5-second monotonic deadline
    $start = hrtime(true);
    $timedOut = false;
    while (true) {
        $status = proc_get_status($proc);
        if (!$status['running']) {
            break;
        }
        if ((hrtime(true) - $start) > $timeoutNs) {
            $timedOut = true;
            break;
        }
        usleep(5000);
    }

    if ($timedOut) {
        echo "FAIL (pipe after exit): child process timed out waiting for exit\n";
        fclose($pipes[1]);
        proc_terminate($proc);
        proc_close($proc);
    } else {
        $term = Terminal::fromStreams($pipes[1]);
        assertThrowsEof(fn() => $term->readKey(Duration::fromMilliseconds(50)), 'pipe after exit call 1');
        assertThrowsEof(fn() => $term->readKey(Duration::fromMilliseconds(50)), 'pipe after exit call 2');
        fclose($pipes[1]);
        proc_close($proc);
    }
}

// 3. Finite memory input at EOF
foreach (['default' => null, 'zero' => Duration::fromSeconds(0), 'positive' => Duration::fromMilliseconds(30)] as $tname => $timeout) {
    $fp = fopen('php://temp', 'r+');
    fwrite($fp, "\xE2");
    rewind($fp);

    $term = Terminal::fromStreams($fp);
    assertThrowsEof(fn() => $timeout === null ? $term->readKey() : $term->readKey($timeout), "memory EOF $tname call 1");
    assertThrowsEof(fn() => $timeout === null ? $term->readKey() : $term->readKey($timeout), "memory EOF $tname call 2");
    fclose($fp);
}

// 4. Contrasting control: writer remains open, partial character times out, continuation bytes arrive
[$input, $writer] = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);
fwrite($writer, "\xE2");
$term = Terminal::fromStreams($input);

$k1 = $term->readKey(Duration::fromMilliseconds(30));
var_dump($k1 === null);

fwrite($writer, "\x82\xAC");
$k2 = $term->readKey(Duration::fromMilliseconds(50));
var_dump($k2 === "€");

fclose($writer);
fclose($input);

// 5. Empty EOF control
[$input, $writer] = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);
fclose($writer);
$term = Terminal::fromStreams($input);
assertThrowsEof(fn() => $term->readKey(), 'empty socket default timeout');
assertThrowsEof(fn() => $term->readKey(Duration::fromSeconds(0)), 'empty socket zero timeout');
fclose($input);

// 6. Complete character followed by EOF control
[$input, $writer] = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);
fwrite($writer, "A");
fclose($writer);
$term = Terminal::fromStreams($input);
var_dump($term->readKey(Duration::fromMilliseconds(50)) === "A");
assertThrowsEof(fn() => $term->readKey(Duration::fromMilliseconds(50)), 'complete char then EOF call 2');
fclose($input);

[$input, $writer] = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);
fwrite($writer, "€");
fclose($writer);
$term = Terminal::fromStreams($input);
var_dump($term->readKey(Duration::fromMilliseconds(50)) === "€");
assertThrowsEof(fn() => $term->readKey(Duration::fromMilliseconds(50)), 'complete multibyte then EOF call 2');
fclose($input);

?>
--EXPECT--
PASS (closed socket 2-byte \xC3 default call 1)
PASS (closed socket 2-byte \xC3 default call 2)
PASS (closed socket 2-byte \xC3 zero call 1)
PASS (closed socket 2-byte \xC3 zero call 2)
PASS (closed socket 2-byte \xC3 positive call 1)
PASS (closed socket 2-byte \xC3 positive call 2)
PASS (closed socket 3-byte \xE2 default call 1)
PASS (closed socket 3-byte \xE2 default call 2)
PASS (closed socket 3-byte \xE2 zero call 1)
PASS (closed socket 3-byte \xE2 zero call 2)
PASS (closed socket 3-byte \xE2 positive call 1)
PASS (closed socket 3-byte \xE2 positive call 2)
PASS (closed socket 3-byte \xE2\x82 default call 1)
PASS (closed socket 3-byte \xE2\x82 default call 2)
PASS (closed socket 3-byte \xE2\x82 zero call 1)
PASS (closed socket 3-byte \xE2\x82 zero call 2)
PASS (closed socket 3-byte \xE2\x82 positive call 1)
PASS (closed socket 3-byte \xE2\x82 positive call 2)
PASS (closed socket 4-byte \xF0 default call 1)
PASS (closed socket 4-byte \xF0 default call 2)
PASS (closed socket 4-byte \xF0 zero call 1)
PASS (closed socket 4-byte \xF0 zero call 2)
PASS (closed socket 4-byte \xF0 positive call 1)
PASS (closed socket 4-byte \xF0 positive call 2)
PASS (closed socket 4-byte \xF0\x9F\x92 default call 1)
PASS (closed socket 4-byte \xF0\x9F\x92 default call 2)
PASS (closed socket 4-byte \xF0\x9F\x92 zero call 1)
PASS (closed socket 4-byte \xF0\x9F\x92 zero call 2)
PASS (closed socket 4-byte \xF0\x9F\x92 positive call 1)
PASS (closed socket 4-byte \xF0\x9F\x92 positive call 2)
PASS (pipe after exit call 1)
PASS (pipe after exit call 2)
PASS (memory EOF default call 1)
PASS (memory EOF default call 2)
PASS (memory EOF zero call 1)
PASS (memory EOF zero call 2)
PASS (memory EOF positive call 1)
PASS (memory EOF positive call 2)
bool(true)
bool(true)
PASS (empty socket default timeout)
PASS (empty socket zero timeout)
bool(true)
PASS (complete char then EOF call 2)
bool(true)
PASS (complete multibyte then EOF call 2)
